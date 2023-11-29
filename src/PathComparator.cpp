#include "PathComparator.hpp"

void PathComparator::compareStructures(sfp path1, sfp path2) noexcept{
    bool one_is_dir = std::filesystem::is_directory(path1);
    bool two_is_dir = std::filesystem::is_directory(path2);

    if(one_is_dir && two_is_dir){
        // one and two are dirs
    }
    else if(!one_is_dir && two_is_dir){
        // one is file, two is dir
    }
    else if(one_is_dir && !two_is_dir){
        // one is dir, two is file
    }
    else if(!one_is_dir && !two_is_dir){
        // one and two are files
    }
}