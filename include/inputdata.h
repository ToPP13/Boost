//
// Created by kirill on 02.12.2020.
//

#pragma once

#include <string>

#include <boost/filesystem.hpp>


typedef std::vector<boost::filesystem::path> path_vec;
typedef std::vector<std::string> hash;



class InputData {
public:
    InputData():
            target_directories({}),
            exclude_directories({}),
            depth_level(0),
            file_size(0),
            block_size(5),
            file_mask(""),
            hash_algorithm("crc32"){};
public:
    path_vec target_directories;
    path_vec exclude_directories;
    uint depth_level;
    uint file_size;
    uint block_size;
    std::string file_mask;
    std::string hash_algorithm;
};