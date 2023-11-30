#include "PathComparator.hpp"

std::map<PathComparator::sfp, PathComparator::DirectoryContent> PathComparator::old_structure = 
    std::map<PathComparator::sfp, PathComparator::DirectoryContent>();
    
std::map<PathComparator::sfp, PathComparator::DirectoryContent> PathComparator::new_structure = 
    std::map<PathComparator::sfp, PathComparator::DirectoryContent>();



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

std::map<PathComparator::sfp, PathComparator::DirectoryContent> PathComparator::readFileStructure(sfp path){
    if(!fs::is_regular_file(path)){
        throw std::runtime_error("path is not a file!");
    }

    std::map<sfp, DirectoryContent> structure;

    std::fstream file(path, std::ios::in);
    std::string buffer;

    DirectoryContent dir_content;
    
    sfp dir_name;
    sfp relative_dir_name;

    while(std::getline(file,buffer)){
        if(buffer.empty()) continue;

        if(buffer.find(PathComparator::directoryNamePrefix, 0) != std::string::npos){
            // add new Directory
            if(!relative_dir_name.empty()){
                // first add previous Directory to structure if not empty
                try
                {
                    structure.insert(std::pair<sfp, DirectoryContent>(dir_name, DirectoryContent(dir_content)));
                }
                catch(const std::exception& e)
                {
                    throw std::runtime_error("error while inserting dir_content (" + std::string(e.what()) + ")");
                }
            }
            dir_content = DirectoryContent();
            auto current_path = buffer.substr(std::string(PathComparator::directoryNamePrefix).size());
            if(relative_dir_name.empty())
                relative_dir_name = current_path;
            dir_name = fs::relative(current_path, relative_dir_name);
            
            // printf("NAME: %s\n", dir_name.string().c_str());
        }
        else if(buffer[0] != ' '){
            // printf("%s\n", buffer.c_str());
            // add file/path to current DirectoryStructure
            auto str = PathComparator::removeMultipleSpaces(buffer);
            auto splited = PathComparator::splitStringBy(str, " ");
            // [0] - date, [1] - time, [2] - <DIR> / size, [3] - name
            if(splited.size() == 4){
                if(splited[2] == PathComparator::dirMarker){
                    DirData dir_data;
                    dir_data.mod_date = splited[0];
                    dir_data.mod_time = splited[1];
                    try
                    {
                        dir_content.directories.insert(std::pair<std::string, DirData>(splited[3], dir_data));
                    }
                    catch(const std::exception& e)
                    {
                        throw std::runtime_error("error while inserting dir_data (" + std::string(e.what()) + ")");
                    }
                }
                else{
                    FileData file_data;
                    file_data.mod_date = splited[0];
                    file_data.mod_time = splited[1];
                    file_data.size = std::stoull(splited[2]);
                    try
                    {
                        dir_content.files.insert(std::pair<std::string, FileData>(splited[3], file_data));
                    }
                    catch(const std::exception& e)
                    {
                        throw std::runtime_error("error while inserting file_data (" + std::string(e.what()) + ")");
                    }
                }
            }
            else{
                throw std::runtime_error("cannot interpret line: '" + buffer + "'! Expected: [date] [time] [<DIR>/size] [name]" );
            }
        }
    }
    file.close();
    try
    {
        structure.insert(std::pair<sfp, DirectoryContent>(dir_name, DirectoryContent(dir_content)));
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("error while inserting dir_content (" + std::string(e.what()) + ")");
    }
    return structure;
}

std::map<PathComparator::sfp, PathComparator::DirectoryContent> PathComparator::readDirStructure(sfp path){
    if(!fs::is_directory(path)){
        throw std::runtime_error("path is not a directory!");
    }
    
    std::map<sfp, DirectoryContent> structure;

    return structure;
}


void PathComparator::compareStructuresData(){
    // look for deleted files
    for(const auto &i : PathComparator::old_structure){
        auto it = PathComparator::new_structure.find(i.first);
        if(it == PathComparator::new_structure.end()){
            // dir not found
            printf("%s was deleted\n", i.first.string().c_str());
        }
        else{
            auto dir_content = PathComparator::new_structure.at(i.first);
            // for(const auto &j : i.second.directories){
            //     auto it = dir_content.directories.find(j.first);
            //     if(it == dir_content.directories.end()){
            //         // dir not found
            //         printf("%s was deleted\n", j.first.c_str());
            //     }
            // }
            for(const auto &j : i.second.files){
                auto it = dir_content.files.find(j.first);
                if(it == dir_content.files.end()){
                    // dir not found
                    printf("%s was deleted\n", j.first.c_str());
                }
                else{
                    auto file_old = j.second;
                    auto file_new = dir_content.files.at(j.first);
                    if(file_old.mod_time != file_new.mod_time || file_old.mod_date != file_new.mod_date){
                        printf("file %s was modified (date)!\n", j.first.c_str());
                    }
                    if(file_old.size != file_new.size){
                        printf("file %s was modified (size)!\n", j.first.c_str());

                    }
                }
            }

        }
    }
    
    // look for new files
    for(const auto &i : PathComparator::new_structure){
        auto it = PathComparator::old_structure.find(i.first);
        if(it == PathComparator::old_structure.end()){
            // dir not found
            printf("%s was added\n", i.first.string().c_str());
        }
        else{
            auto dir_content = PathComparator::old_structure.at(i.first);
            for(const auto &j : dir_content.files){
                auto it = dir_content.files.find(j.first);
                if(it == dir_content.files.end()){
                    // dir not found
                    printf("%s was added\n", j.first.c_str());
                }
            }
        }
    }

}

void PathComparator::compareStructures(sfp osp, sfp nsp) noexcept{
    try
    {
        PathComparator::old_structure = 
            (fs::is_directory(osp) ? PathComparator::readDirStructure(osp) : PathComparator::readFileStructure(osp));
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "error while reading first structure form: %s (%s)\n", osp.string().c_str(), e.what());
        return;
    }
    
    try
    {
        PathComparator::new_structure = 
            (fs::is_directory(nsp) ? PathComparator::readDirStructure(nsp) : PathComparator::readFileStructure(nsp));
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "error while reading second structure form: %s (%s)\n", nsp.string().c_str(), e.what());
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