#include "../include/lib.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;


InputData parse_command_line(int argc, const char *argv[])
{
    InputData res;
    try {
        po::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "This screen")
                ("input,i",  po::value<path_vec >(), "Input directories")
                ("exclude,e",  po::value<path_vec>(), "Excluded inner directories")
                ("level,l",  po::value<uint>(), "Scan level")
                ("size,s",  po::value<uint>(), "Minimum files size")
                ("mask,m",  po::value<std::string>(), "File mask")
                ("block,b",  po::value<uint>(), "Block size")
                ("alg,a",  po::value<std::string>(), "Hash algorithm");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
            std::cout << desc << '\n';
        if (vm.count("input"))
        {
            res.target_directories = vm["input"].as<path_vec>();
            for (auto f : res.target_directories)
                std::cout << "input: " << f  << std::endl;
        }
        if (vm.count("exclude"))
        {
            res.exclude_directories = vm["exclude"].as<path_vec>();
            for (auto f : res.exclude_directories)
                std::cout << "exclude: " << f  << std::endl;
        }

        if (vm.count("level"))
        {
            res.depth_level = vm["level"].as<uint>();
            std::cout << "level: " << res.depth_level << std::endl;

        }
        if (vm.count("size"))
        {
            res.file_size = vm["size"].as<uint>();
            std::cout << "size: " << res.file_size << std::endl;

        }
        if (vm.count("mask"))
        {
            res.file_mask = vm["mask"].as<std::string>();
            std::cout << "mask: " << res.file_mask << std::endl;

        }
        if (vm.count("block"))
        {
            res.block_size = vm["block"].as<uint>();
            std::cout << "block: " << res.block_size << std::endl;
        }
        if (vm.count("alg"))
        {
            res.hash_algorithm = vm["alg"].as<std::string>();
            std::cout << "alg: " << res.hash_algorithm << std::endl;
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return res;
}


int main(int argc, const char *argv[])
{
    InputData data = parse_command_line(argc, argv);
    FileParser fp;
    fp.find_dublicates(data);
    return 0;

}
