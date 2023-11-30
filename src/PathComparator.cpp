#include "PathComparator.hpp"

std::vector<PathComparator::DirectoryContent> PathComparator::structure1 = 
    std::vector<PathComparator::DirectoryContent>();
    
std::vector<PathComparator::DirectoryContent> PathComparator::structure2 = 
    std::vector<PathComparator::DirectoryContent>();



inline std::string PathComparator::removeMultipleSpaces(std::string str){
    auto spacePos(str.find("  "));
    while (spacePos != std::string::npos)
    {
        auto strtmp(str.replace(spacePos, 2, " "));
        str = strtmp;
        spacePos = str.find("  ");
    }
    return str;
}

inline std::vector<std::string> PathComparator::splitStringBy(std::string str, std::string split_by){
    std::vector<std::string> vec;
    while(!str.empty()){
        auto pos(str.find(split_by));
        vec.push_back(str.substr(0, pos));
        if(pos == std::string::npos) 
            break;
        str.erase(0, pos + split_by.size());
    }
    return vec;
}

std::vector<PathComparator::DirectoryContent> PathComparator::readFileStructure(sfp path){
    if(!std::filesystem::is_regular_file(path)){
        throw std::runtime_error("path is not a file!");
    }

    std::vector<DirectoryContent> structure;

    std::fstream file(path, std::ios::in);
    std::string buffer;

    DirectoryContent dirContent;
    dirContent.path.clear();
    while(std::getline(file,buffer)){
        if(buffer.empty()) continue;

        if(buffer.find(PathComparator::directoryNamePrefix, 0) != std::string::npos){
            // add new Directory
            if(!dirContent.path.empty())
                // first add previous Directory to structure if not empty
                structure.push_back(DirectoryContent(dirContent));

            dirContent = DirectoryContent();
            dirContent.path = buffer.substr(std::string(PathComparator::directoryNamePrefix).size());
        }
        else if(buffer[0] != ' '){
            // add file/path to current DirectoryStructure
            auto str = PathComparator::removeMultipleSpaces(buffer);
            auto splited = PathComparator::splitStringBy(str, " ");
            // [0] - date, [1] - time, [2] - <DIR> / size, [3] - name
            if(splited.size() == 4){
                if(splited[2] == PathComparator::dirMarker){
                    DirData dd;
                    dd.name = splited[3];
                    dd.mod_date = splited[0];
                    dd.mod_time = splited[1];
                    dirContent.directories.push_back(dd);
                }
                else{
                    FileData fd;
                    fd.name = splited[3];
                    fd.mod_date = splited[0];
                    fd.mod_time = splited[1];
                    fd.size = std::stoull(splited[2]);
                    dirContent.files.push_back(fd);
                }
            }
            else{
                throw std::runtime_error("cannot interpret line: '" + buffer + "'! Expected: [date] [time] [<DIR>/size] [name]" );
            }
        }
    }
    file.close();
    return structure;
}

std::vector<PathComparator::DirectoryContent> PathComparator::readDirStructure(sfp path){
    if(!std::filesystem::is_directory(path)){
        throw std::runtime_error("path is not a directory!");
    }
    
    std::vector<DirectoryContent> structure;

    return structure;
}


bool PathComparator::compareDirectoryContentObj(const DirectoryContent &a, const DirectoryContent &b){

}
bool PathComparator::compareDirDataObj(const DirData &a, const DirData &b){

}
bool PathComparator::compareFileDataObj(const FileData &a, const FileData &b){

}


void PathComparator::compareStructuresData(){

}

void PathComparator::compareStructures(sfp path1, sfp path2) noexcept{
    try
    {
        if(std::filesystem::is_directory(path1))
            PathComparator::structure1 = PathComparator::readDirStructure(path1);
        else
            PathComparator::structure1 = PathComparator::readFileStructure(path1);
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "error while reading first structure form: %s (%s)\n", path1.string().c_str(), e.what());
        return;
    }
    
    try
    {
        if(std::filesystem::is_directory(path2))
            PathComparator::structure2 = PathComparator::readDirStructure(path2);
        else
            PathComparator::structure2 = PathComparator::readFileStructure(path2);
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "error while reading second structure form: %s (%s)\n", path2.string().c_str(), e.what());
        return;
    }

    try
    {
        PathComparator::compareStructuresData();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "error while comparing both readed structures (%s)\n", e.what());
        return;
    }
    
}