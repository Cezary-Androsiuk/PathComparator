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

    struct FileData
    {
        std::string mod_date;
        std::string mod_time;
        size_t size;
    };

    struct DirData
    {
        std::string mod_date;
        std::string mod_time;
    };
    
    
    struct DirectoryContent
    {
        std::map<std::string, FileData> files;
        std::map<std::string, DirData> directories;
    };
    
    static std::map<sfp, DirectoryContent> old_structure;
    static std::map<sfp, DirectoryContent> new_structure;


    static inline std::string removeMultipleSpaces(std::string str);
    static inline std::vector<std::string> splitStringBy(std::string str, std::string split_by);

    static std::map<sfp, DirectoryContent> readFileStructure(sfp path);
    static std::map<sfp, DirectoryContent> readDirStructure(sfp path);

    static void compareStructuresData();

public:
    static void compareStructures(sfp oldStructurePath, sfp newStructurePath) noexcept;
};