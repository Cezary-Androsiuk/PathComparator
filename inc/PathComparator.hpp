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
    static constexpr const char *deleteElementsFile = "delete_files_list.txt";
    static constexpr const char *updateElementsFile = "update_files_list.txt";

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

    static void printStructure(const DirectoryStructure &_struct, bool new_line = false) noexcept;
    static inline std::string removeMultipleSpaces(std::string str);
    static inline std::vector<std::string> splitStringBy(std::string str, std::string split_by);

    static DirectoryStructure readFileStructure(sfp path);
    static DirectoryStructure readDirStructure(sfp path);

    static void compareFiles(
        DirectoryElement l_element,
        DirectoryElement r_element,
        std::string name,
        bool save_changes_to_file);

    static void compareStructuresData(bool save_changes_to_file);

public:
    static void printStructure(std::filesystem::path structurePath, bool new_line = false) noexcept;
    static void compareStructures(std::filesystem::path oldStructurePath, sfp newStructurePath, bool save_changes_to_file = false) noexcept;
};