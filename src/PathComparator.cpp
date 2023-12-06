#include "PathComparator.hpp"

PathComparator::DirectoryStructure PathComparator::l_struct = PathComparator::DirectoryStructure();

PathComparator::DirectoryStructure PathComparator::r_struct = PathComparator::DirectoryStructure();

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
            // add new Directory
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

            // printf("NAME: %s\n", dir_name.string().c_str());
        }
        else if (buffer[0] != ' ')
        {
            // printf("%s\n", buffer.c_str());
            // add file/path to current DirectoryStructure
            auto str = removeMultipleSpaces(buffer);
            auto splited = splitStringBy(str, " ");
            // [0] - date, [1] - time, [2] - <DIR> / size, [3] - name
            if (splited.size() == 4)
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
                try
                {
                    dir_elements.insert(std::pair<std::string, DirectoryElement>(splited[3], element));
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error("error while inserting element (" + std::string(e.what()) + ")");
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
    throw std::runtime_error("path is not a directory!");

    DirectoryStructure structure;

    return structure;
}

void PathComparator::compareFiles(DirectoryElement l_element, DirectoryElement r_element, std::string name)
{
    if (l_element.is_directory || r_element.is_directory)
        return;
    bool print_modified = false;
    if (l_element.mod_time != r_element.mod_time || l_element.mod_date != r_element.mod_date)
    {
        printf("file %s was modified (date", name.c_str());
        print_modified = true;
    }
    if (l_element.size != r_element.size)
    {
        if (print_modified)
            printf(" and size");
        else
        {
            printf("file %s was modified (size", name.c_str());
            print_modified = true;
        }
    }
    if (print_modified)
        printf(")\n");
}

void PathComparator::compareStructuresData()
{
    if (PathComparator::l_struct.empty() || PathComparator::r_struct.empty())
    {
        printf("at least one structure is empty!\n");
        return;
    }

    // look for deleted files
    for (const auto &i : PathComparator::l_struct)
    {
        if (i.first == "." || i.first == "..")
            continue;
        auto it_structure = PathComparator::r_struct.find(i.first);
        if (it_structure == PathComparator::r_struct.end())
        {
            printf("'%s' directory was deleted\n", i.first.string().c_str());
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
                continue;
            }

            auto l_element = j.second;
            auto r_element = it_structure->second.at(j.first);
            PathComparator::compareFiles(l_element, r_element, j.first);
        }
    }

    // look for new files
    for (const auto &i : PathComparator::r_struct)
    {
        if (i.first == "." || i.first == "..")
            continue;
        auto it_structure = PathComparator::l_struct.find(i.first);
        if (it_structure == PathComparator::l_struct.end())
        {
            // dir not found
            printf("'%s' directory was added\n", i.first.string().c_str());
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
                continue;
            }
        }
    }
}

void PathComparator::printStructure(const DirectoryStructure &structure)
{
    for (const auto &i : structure)
    {
        for (const auto &j : i.second)
        {
            if (j.first == "." || j.first == "..")
                continue;
            printf(
                "[%s]: %s%s: %s %s %llu\n",
                i.first.string().c_str(),
                j.first.c_str(),
                (j.second.is_directory ? "/" : ""),
                j.second.mod_date.c_str(),
                j.second.mod_time.c_str(),
                j.second.size);
        }
    }
}

void PathComparator::compareStructures(sfp osp, sfp nsp) noexcept
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
        PathComparator::compareStructuresData();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "error while comparing both readed structures (%s)\n", e.what());
        return;
    }
}