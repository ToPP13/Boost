//
// Created by kirill on 02.12.2020.
//

#include "../include/fileparser.h"
#include "iostream"

using namespace boost::filesystem;
using std::cout;
using std::endl;

#include <boost/regex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>


using boost::uuids::detail::md5;

path_vec FileParser::get_target_dirs(const path_vec & target_directories, const path_vec &exclude_directories, int depth_level)
{
    path_vec all_dirs;
    for (auto & td: target_directories)
    {
        if (is_directory(td))
        {
            auto it = find(exclude_directories.begin(), exclude_directories.end(), td);
            if (it == exclude_directories.end())
                all_dirs.push_back(td);
        }
        recursive_directory_iterator dir(td), end;
        while (dir != end)
        {
            // make sure we don't recurse into certain directories
            // note: maybe check for is_directory() here as well...
            if (!is_directory(dir->path()))
            {
                ++dir;
                continue;
            }

            if (dir.depth() >= depth_level)
            {
                ++dir;
                continue;
            }
            auto it = find(exclude_directories.begin(), exclude_directories.end(), dir->path().filename());
            if (it == exclude_directories.end())
                all_dirs.push_back(*dir);
            ++dir;
        }
    }
    return all_dirs;
}

path_vec FileParser::get_target_files(const path_vec & target_directories, const std::string file_mask, const uint size_threshold)
{
    path_vec all_files;

    const boost::regex filter(file_mask);

    for (const auto & td: target_directories)
    {
        directory_iterator it(td), eod;

        while (it != eod)
        {
            if(!is_regular_file(it->path()))
            {
                it++;
                continue;
            }
            if (size_threshold != 0)
            {
                if (file_size(it->path()) < size_threshold)
                {
                    it++;
                    continue;
                }
            }

            if (!file_mask.empty())
            {
                boost::smatch what;
                if( boost::regex_search( it->path().filename().string(), what, filter ) )
                {
                    it++;
                    continue;
                }
            }
            all_files.push_back(it->path());
            it++;
        }
    }
    return all_files;
}


std::string FileParser::calc_hash_crc32(const char * word_to_hash, const uint block_size)
{
    std::string str_hash = "";
    boost::crc_32_type result;
    result.process_bytes(word_to_hash, block_size);
    str_hash = std::to_string(result.checksum());
    return str_hash;
}

std::string FileParser::calc_hash_md5(const char * word_to_hash, const uint block_size)
{
    md5 hash;
    md5::digest_type digest;
    hash.process_bytes(word_to_hash, block_size);
    hash.get_digest(digest);
    std::string str_digest = "";
    const auto charDigest = reinterpret_cast<const char *>(&digest);
    boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(str_digest));
    return str_digest;
}

std::string FileParser::calc_hash(const char * word_to_hash, const uint block_size, const std::string hash_alg)
{
    std::string result_hash = "";
    if (hash_alg == "crc32")
    {
        result_hash = calc_hash_crc32(word_to_hash, block_size);
    }
    else if (hash_alg == "md5")
    {
        result_hash = calc_hash_md5(word_to_hash, block_size);
    }

//    for (uint i=0;i<block_size;i++)
//        cout << word_to_hash[i];
//    cout << " with " << hash_alg << " into " << result_hash << endl;
    return result_hash;
}

std::string FileParser::calc_hash_for_file_block(path file, uint position, const uint block_size, const std::string hash_alg)
{
    std::string result_hash = "";
    std::ifstream is (file.string(), std::ifstream::binary);
    if (is)
    {
        is.seekg(position);
        char * buffer = new char [block_size];

        is.read (buffer,block_size);
        // read data as a block:

        if (!is)
        {
            for (uint i = is.gcount()-1;i<block_size;i++)
                buffer[i]='0';
        }

        is.close();
        return calc_hash(buffer, block_size, hash_alg);
    }
    return result_hash;
}

bool FileParser::equal_hashes(CustomFile & targetFile, CustomFile & refFile, const uint block_size, const std::string & hash_alg)
{
    bool isDublicate = true;
    uint i = 0;
    std::string cur_target_hash = "";
    std::string cur_ref_hash = "";
    while (isDublicate)
    {
        uint cur_file_size = i*block_size;
        if (cur_file_size >= targetFile.file_size || cur_file_size >= refFile.file_size)
            break;
        // get hash from ref
        if (i<targetFile.file_hash.size())
        {
            cur_target_hash = targetFile.file_hash[i];
        }
        else
        {
            cur_target_hash = calc_hash_for_file_block(targetFile.file_path, i * block_size, block_size, hash_alg);
            targetFile.file_hash.push_back(cur_target_hash);
        }

        // get hash from target
        if (i<refFile.file_hash.size())
            cur_ref_hash = refFile.file_hash[i];
        else
        {
            cur_ref_hash = calc_hash_for_file_block(refFile.file_path, i * block_size, block_size, hash_alg);
            refFile.file_hash.push_back(cur_ref_hash);
        }

        // compare
        if (cur_target_hash != cur_ref_hash)
        {
            isDublicate = false;
            break;
        }
        i++;
    }
    return isDublicate;
}

void FileParser::find_dublicates(const InputData & data)
{
    auto result_dirs = get_target_dirs(data.target_directories,
                                    data.exclude_directories,
                                    data.depth_level);
//    for (auto d: all_dirs)
//        std::cout << d.string() << std::endl;

    auto result_files = get_target_files(result_dirs, data.file_mask, data.file_size);
//    for (auto f: result_files)
//        std::cout << f << std::endl;

    // map that stores vector of dublicates for each path
    std::map<path, std::vector<CustomFile>> file_dublicates;

    // need file_data because CustomFile has vector inside and cause of that
    // it is impossible/hard to make CustomFile as key
    std::map<path, CustomFile> file_data;


    for (auto target_file: result_files)
    {
//        cout << "Process: " << target_file.string() << endl;
        CustomFile cur_file_data;
        cur_file_data.file_size = file_size(target_file);
        cur_file_data.file_path = target_file;

        hash new_file_hash;
        hash exist_file_hash;
        bool is_dublicate = false;
        for (auto & ref_file_pair : file_dublicates)
        {
            if (equal_hashes(cur_file_data, file_data[ref_file_pair.first], data.block_size, data.hash_algorithm))
            {
                file_dublicates[ref_file_pair.first].push_back(cur_file_data);
                is_dublicate = true;
                break;
            }
        }
        if (!is_dublicate)
        {
            file_dublicates[target_file] = {cur_file_data};
            file_data[target_file] = cur_file_data;
        }
    }

    cout << endl << endl << endl;
    for (const auto & pair : file_dublicates)
    {
        for (const auto & f : pair.second)
        {
            cout << absolute(f.file_path).string() << endl;
        }
        cout << endl;
    }
}

