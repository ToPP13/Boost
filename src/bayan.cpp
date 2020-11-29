#include "../include/lib.h"


#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

void set_bulk(size_t bulk) {
    std::cout << "bulk size is " << bulk << std::endl;
}


class InfoData {
public:
    InfoData():
    target_directories({"."}),
    exclude_directories({}),
    depth_level(0),
    file_size(0),
    block_size(5),
    file_mask(""),
    hash_algorithm(""){};
public:
    std::vector<std::string> target_directories;
    std::vector<std::string> exclude_directories;
    uint depth_level;
    uint file_size;
    uint block_size;
    std::string file_mask;
    std::string hash_algorithm;
};


InfoData parse_command_line(int argc, const char *argv[])
{
    InfoData res;
    try {
        po::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "This screen")
                ("input,i",  po::value<std::vector<std::string> >(), "Input directories")
                ("exception,e",  po::value<std::vector<std::string> >(), "Excluded inner directories")
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
            res.target_directories = vm["input"].as<std::vector<std::string>>();
            for (auto f : res.target_directories)
                std::cout << "input: " << f  << std::endl;
        }
        if (vm.count("exception"))
        {
            res.exclude_directories = vm["input"].as<std::vector<std::string>>();
            for (auto f : res.exclude_directories)
                std::cout << "input: " << f  << std::endl;
        }

        if (vm.count("level"))
        {
            res.depth_level = vm["level"].as<uint>();
            std::cout << "level: " << vm["level"].as<uint>() << std::endl;

        }
        if (vm.count("size"))
        {
            res.file_size = vm["size"].as<uint>();
            std::cout << "size: " << vm["size"].as<uint>() << std::endl;

        }
        if (vm.count("mask"))
        {
            res.file_mask = vm["mask"].as<std::string>();
            std::cout << "mask: " << vm["mask"].as<std::string>() << std::endl;

        }
        if (vm.count("block"))
        {
            res.block_size = vm["block"].as<uint>();
            std::cout << "block: " << vm["block"].as<uint>() << std::endl;
        }
        if (vm.count("alg"))
        {
            res.hash_algorithm = vm["alg"].as<std::string>();
            std::cout << "alg: " << vm["alg"].as<std::string>() << std::endl;
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return res;
}


int main(int argc, const char *argv[])
{
    InfoData data = parse_command_line(argc, argv);


}
