#pragma once

#include <cstdio>
#include <filesystem>

class PathComparator
{
    typedef std::filesystem::path sfp;

    
public:
    static void compareStructures(sfp path1, sfp path2) noexcept;
};