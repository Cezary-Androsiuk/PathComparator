#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

class PathComparator
{
    typedef fs::path sfp;

    static constexpr const char *directoryNamePrefix = " Directory of ";
    static constexpr const char *dirMarker = "<DIR>";

    struct DirectoryElement
    {
        std::string mod_date;
        std::string mod_time;
        size_t size;
        bool is_directory;
    };

    typedef std::map<std::string, DirectoryElement> DirectoryElements;
    typedef std::map<sfp, DirectoryElements> DirectoryStructure;
    
    static DirectoryStructure l_struct;
    static DirectoryStructure r_struct;


    static inline std::string removeMultipleSpaces(std::string str);
    static inline std::vector<std::string> splitStringBy(std::string str, std::string split_by);

    static DirectoryStructure readFileStructure(sfp path);
    static DirectoryStructure readDirStructure(sfp path);

    static void compareFiles(DirectoryElement l_element, DirectoryElement r_element, std::string name);

    static void compareStructuresData();

public:
    static void printStructure(const DirectoryStructure &_struct);
    static void compareStructures(sfp oldStructurePath, sfp newStructurePath) noexcept;
};