#include "PathComparator.hpp"

PathComparator::DirectoryStructure PathComparator::l_struct = PathComparator::DirectoryStructure();

PathComparator::DirectoryStructure PathComparator::r_struct = PathComparator::DirectoryStructure();

void PathComparator::printStructure(const DirectoryStructure &structure, bool new_line) noexcept
{
    for (const auto &i : structure)
    {
        for (const auto &j : i.second)
        {
            // if (j.first == "." || j.first == "..")
            //     continue;
            try
            {
                printf(
                    "[%s]: %s%s: %s %s %llu\n",
                    i.first.string().c_str(),
                    j.first.c_str(),
                    (j.second.is_directory ? "/" : ""),
                    j.second.mod_date.c_str(),
                    j.second.mod_time.c_str(),
                    j.second.size);   
            }
            catch(const std::exception& e)
            {
                fprintf(stderr, "error while printing structure line (%s)\n", e.what());
            }
        }
    }
    if (new_line)
        printf("\n");
}

inline std::string PathComparator::removeMultipleSpaces(std::string str)
{
    auto spacePos(str.find("  "));
    while (spacePos != std::string::npos)
    {
        auto strtmp(str.replace(spacePos, 2, " "));
        str = strtmp;
        spacePos = str.find("  ");
    }
    return str;
}

inline std::vector<std::string> PathComparator::splitStringBy(std::string str, std::string split_by)
{
    std::vector<std::string> vec;
    while (!str.empty())
    {
        auto pos(str.find(split_by));
        vec.push_back(str.substr(0, pos));
        if (pos == std::string::npos)
            break;
        str.erase(0, pos + split_by.size());
    }
    return vec;
}

PathComparator::DirectoryStructure PathComparator::readFileStructure(sfp path)
{
    if (!fs::is_regular_file(path))
    {
        throw std::runtime_error("path is not a file!");
    }

    DirectoryStructure structure;

    std::fstream file(path, std::ios::in);
    std::string buffer;

    DirectoryElements dir_elements;

    sfp dir_name;
    sfp relative_dir_name;

    while (std::getline(file, buffer))
    {
        if (buffer.empty())
            continue;

        if (buffer.find(directoryNamePrefix, 0) != std::string::npos)
        {
            // add directory name
            if (!relative_dir_name.empty())
            {
                // first add previous Directory to structure if not empty
                try
                {
                    structure.insert(std::pair<sfp, DirectoryElements>(dir_name, DirectoryElements(dir_elements)));
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error("error while inserting dir_elements (" + std::string(e.what()) + ")");
                }
            }
            dir_elements = DirectoryElements();
            auto current_path = buffer.substr(std::string(directoryNamePrefix).size());
            if (relative_dir_name.empty())
                relative_dir_name = current_path;
            dir_name = fs::relative(current_path, relative_dir_name);
        }
        else if (buffer[0] != ' ')
        {
            // add directory elements
            auto str = removeMultipleSpaces(buffer);
            auto splited = splitStringBy(str, " ");
            // [0] - date, [1] - time, [2] - <DIR> / size, [3] - name
            if (splited.size() == 4)
            {
                if (splited[3] == "." || splited[3] == "..")
                    continue;

                try
                {
                    size_t element_size;
                    bool is_directory;
                    if (splited[2] == dirMarker)
                    {
                        element_size = 4096;
                        is_directory = true;
                    }
                    else
                    {
                        element_size = std::stoull(splited[2]);
                        is_directory = false;
                    }

                    DirectoryElement element;
                    element.mod_date = splited[0];
                    element.mod_time = splited[1];
                    element.size = element_size;
                    element.is_directory = is_directory;
                    dir_elements.insert(std::pair<std::string, DirectoryElement>(splited[3], element));
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error("error while inserting element '" + buffer + "' (" + std::string(e.what()) + ")");
                }
            }
            else
            {
                throw std::runtime_error("cannot interpret line: '" + buffer + "'! Expected: [date] [time] [<DIR>/size] [name]");
            }
        }
    }
    file.close();
    try
    {
        structure.insert(std::pair<sfp, DirectoryElements>(dir_name, DirectoryElements(dir_elements)));
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("error while inserting dir_elements (" + std::string(e.what()) + ")");
    }
    return structure;
}

PathComparator::DirectoryStructure PathComparator::readDirStructure(sfp path)
{
    if (!fs::is_directory(path))
    {
        throw std::runtime_error("path is not a directory!");
    }

    DirectoryStructure structure;

    std::string tmp_file = "dir_structure.tmp";
    std::string command = "dir /S /R " + path.string() + " > " + tmp_file;
    int result = std::system(command.c_str());
    if (result)
    {
        printf("error while executing '%s' command with %lld value\n", command.c_str(), result);
        return structure;
    }

    structure = PathComparator::readFileStructure(tmp_file);

    try
    {
        fs::remove(tmp_file);
    }
    catch (const std::exception &e)
    {
        printf("error while deleting %s file\n", tmp_file.c_str());
    }

    return structure;
}

void PathComparator::compareFiles(
    DirectoryElement l_element,
    DirectoryElement r_element,
    std::string name,
    bool save_changes_to_file)
{
    std::fstream file(PathComparator::updateElementsFile, std::ios::app);

    if (l_element.is_directory || r_element.is_directory)
        return;
    bool print_modified = false;
    if (l_element.mod_time != r_element.mod_time || l_element.mod_date != r_element.mod_date)
    {
        printf("'%s' file was modified (modification date", name.c_str());
        print_modified = true;
    }
    if (l_element.size != r_element.size)
    {
        if (print_modified)
            printf(" and size");
        else
        {
            printf("'%s' file was modified (size", name.c_str());
            print_modified = true;
        }
    }
    if (print_modified)
    {
        printf(" was changed)\n");
        file << name << std::endl;
    }
    file.close();
}

void PathComparator::compareStructuresData(bool save_changes_to_file)
{
    if (PathComparator::l_struct.empty() || PathComparator::r_struct.empty())
    {
        printf("at least one structure is empty!\n");
        return;
    }

    try
    {
        if (fs::exists(PathComparator::deleteElementsFile))
            fs::remove(PathComparator::deleteElementsFile);
    }
    catch (const std::exception &e)
    {
        printf("error while deleting %s file (%s)\n", PathComparator::deleteElementsFile, e.what());
    }

    try
    {
        if (fs::exists(PathComparator::updateElementsFile))
            fs::remove(PathComparator::updateElementsFile);
    }
    catch (const std::exception &e)
    {
        printf("error while deleting %s file (%s)\n", PathComparator::updateElementsFile, e.what());
    }

    std::ofstream file(PathComparator::deleteElementsFile);

    // look for deleted files
    for (const auto &i : PathComparator::l_struct)
    {
        // if (i.first == "." || i.first == "..")
        //     continue;
        auto it_structure = PathComparator::r_struct.find(i.first);
        if (it_structure == PathComparator::r_struct.end())
        {
            printf("'%s' directory was deleted\n", i.first.string().c_str());
            file << i.first.string() << std::endl;
            ;
            continue;
        }

        // iterate over elements
        for (const auto &j : i.second)
        {
            auto it_elements = it_structure->second.find(j.first);
            if (it_elements == it_structure->second.end())
            {
                if (j.second.is_directory)
                    continue;

                printf("'%s' file was deleted\n", (i.first / sfp(j.first)).string().c_str());
                file << (i.first / sfp(j.first)).string() << std::endl;
                continue;
            }

            auto l_element = j.second;
            auto r_element = it_structure->second.at(j.first);
            PathComparator::compareFiles(l_element, r_element, (i.first / j.first).string(), save_changes_to_file);
        }
    }

    file.close();
    file.open(PathComparator::updateElementsFile, std::ios::app);

    // look for new files
    for (const auto &i : PathComparator::r_struct)
    {
        // if (i.first == "." || i.first == "..")
        //     continue;
        auto it_structure = PathComparator::l_struct.find(i.first);
        if (it_structure == PathComparator::l_struct.end())
        {
            // dir not found
            printf("'%s' directory was added\n", i.first.string().c_str());
            file << i.first.string() << std::endl;
            ;
            continue;
        }

        // iterate over elements
        for (const auto &j : i.second)
        {
            auto it_elements = it_structure->second.find(j.first);
            if (it_elements == it_structure->second.end())
            {
                if (j.second.is_directory)
                    continue;

                printf("'%s' file was added\n", (i.first / sfp(j.first)).string().c_str());
                file << (i.first / sfp(j.first)).string() << std::endl;
                continue;
            }
        }
    }
    file.close();
}

void PathComparator::printStructure(sfp sp, bool new_line) noexcept
{
    PathComparator::DirectoryStructure structure;
    try
    {
        structure =
            (fs::is_directory(sp) ? PathComparator::readDirStructure(sp) : PathComparator::readFileStructure(sp));
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "error while reading given structure form: %s (%s)\n", sp.string().c_str(), e.what());
        return;
    }
    PathComparator::printStructure(structure, new_line);
}

void PathComparator::compareStructures(sfp osp, sfp nsp, bool save_changes_to_file) noexcept
{
    try
    {
        PathComparator::l_struct =
            (fs::is_directory(osp) ? PathComparator::readDirStructure(osp) : PathComparator::readFileStructure(osp));
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "error while reading first structure form: %s (%s)\n", osp.string().c_str(), e.what());
        return;
    }

    try
    {
        PathComparator::r_struct =
            (fs::is_directory(nsp) ? PathComparator::readDirStructure(nsp) : PathComparator::readFileStructure(nsp));
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "error while reading second structure form: %s (%s)\n", nsp.string().c_str(), e.what());
        return;
    }

    try
    {
        PathComparator::compareStructuresData(save_changes_to_file);
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "error while comparing both readed structures (%s)\n", e.what());
        return;
    }
}