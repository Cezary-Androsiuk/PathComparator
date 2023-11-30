#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

class PathComparator
{
    typedef std::filesystem::path sfp;

    static constexpr const char *directoryNamePrefix = " Directory of ";
    static constexpr const char *dirMarker = "<DIR>";

    struct FileData
    {
        std::string name;
        std::string mod_date;
        std::string mod_time;
        size_t size;
    };

    struct DirData
    {
        std::string name;
        std::string mod_date;
        std::string mod_time;
    };
    
    
    struct DirectoryContent
    {
        sfp path;
        std::vector<FileData> files;
        std::vector<DirData> directories;
    };
    
    static inline std::string removeMultipleSpaces(std::string str);
    static inline std::vector<std::string> splitStringBy(std::string str, std::string split_by);

    static std::vector<DirectoryContent> structure1;
    static std::vector<DirectoryContent> structure2;

    static std::vector<DirectoryContent> readFileStructure(sfp path);
    static std::vector<DirectoryContent> readDirStructure(sfp path);
    
    static bool compareDirectoryContentObj(const DirectoryContent &a, const DirectoryContent &b);
    static bool compareDirDataObj(const DirData &a, const DirData &b);
    static bool compareFileDataObj(const FileData &a, const FileData &b);

    static void compareStructuresData() noexcept;

public:
    static void compareStructures(sfp path1, sfp path2) noexcept;
};