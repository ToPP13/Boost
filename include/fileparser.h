//
// Created by kirill on 02.12.2020.
//

#include "inputdata.h"
#include <string>

#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#pragma once


struct CustomFile
{
    boost::filesystem::path file_path;
    hash file_hash;
    uint file_size;
    bool operator<( const CustomFile& other) const
    {
        if ( file_path == other.file_path )
        {
            return file_path < other.file_path;
        }

        return file_path < other.file_path;
    }
};

typedef std::vector<CustomFile> file_list;

class FileParser
{
public:
    void find_dublicates(const InputData & data);
private:

    std::string calc_hash(const char * word_to_hash, const uint block_size, const std::string hash_alg);
    std::string calc_hash_crc32(const char * word_to_hash, const uint block_size);
    std::string calc_hash_md5(const char * word_to_hash, const uint block_size);

    path_vec get_target_dirs(const path_vec & target_directories,
                             const path_vec & exclude_directories,
                             int depth_level);

    path_vec get_target_files(const path_vec & target_directories,
                              std::string file_mask,
                              uint file_size);

    std::string calc_hash_for_file_block(boost::filesystem::path file, uint position, const uint block_size, const std::string hash_alg);
    bool equal_hashes(CustomFile & targetFile, CustomFile & refFile, const uint block_size, const std::string & hash_alg);
};