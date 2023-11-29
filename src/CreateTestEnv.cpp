#include "CreateTestEnv.hpp"

const char *CreateTestEnv::folder_names[] = {
    "Documents", "Downloads", "Pictures", "Videos", "Music", "Projects", "Work", "School",
    "Personal", "Code", "Games", "Travel", "Family", "Vacations", "Archive", "Temp", "Backup",
    "OldFiles", "Reports", "Logs", "Desktop", "Apps", "Home", "Office", "Expenses", "Invoices",
    "Receipts", "Books", "Articles", "Manuals", "Tutorials", "Notes", "Research", "Clients",
    "Contacts", "Events", "Holidays", "Health", "Fitness", "Recipes", "Shopping", "Wishlist",
    "Gadgets", "Technology", "Reviews", "Art", "Photography", "Podcasts", "Movies", "TVShows",
    "Anime", "Sports", "Hobbies", "Crafts", "DIY", "Gardening", "Pets", "Finances", "Investments",
    "Retirement", "Taxes", "Mortgage", "Loans", "Insurance", "Vehicles", "Cars", "Bikes", "TravelPlans",
    "BucketList", "Friends", "Social", "Networking", "Forums", "Subscriptions", "Memberships",
    "Passwords", "Security", "Updates", "Licenses", "Certificates", "Training", "Courses",
    "Workshops", "Conferences", "Seminars", "Meetings", "Appointments", "Calendar", "Birthdays",
    "Anniversaries", "Milestones", "Achievements", "Rewards", "Recognition", "Goals", "Resolutions",
    "Challenges", "Reflections", "Dreams", "Ideas"
};

CreateTestEnv::sfp CreateTestEnv::current_path = sfp();
CreateTestEnv::sfp CreateTestEnv::env_path = sfp();

void CreateTestEnv::start()
{
    CreateTestEnv::current_path = std::filesystem::current_path();
    if (std::filesystem::exists(CreateTestEnv::env_path))
    {
        std::error_code ec;
        std::uintmax_t n{std::filesystem::remove_all(CreateTestEnv::env_path, ec)};
        if (!ec)
            fprintf(stdout, "Deleted %llu files/directories from old %s directory\n", n, CreateTestEnv::env_path.string().c_str());
        else
            fprintf(stderr, "Error during deleting structure %s\nerror code: %d\n error mess: %s\n",
                    CreateTestEnv::env_path.string().c_str(), ec.value(), ec.message().c_str());
    }

    if (!std::filesystem::create_directory(CreateTestEnv::env_path))
    {
        fprintf(stderr, "Can not create env directory in %s\n", CreateTestEnv::env_path.string().c_str());
        throw 1;
    }

    std::srand(std::time(nullptr));
}

void CreateTestEnv::stop()
{
    printf("Created structure:\n");
    for (const auto &subdir : std::filesystem::recursive_directory_iterator(CreateTestEnv::env_path))
        printf("\t%s\n", subdir.path().string().c_str());

    std::filesystem::current_path(CreateTestEnv::current_path);
}

std::string CreateTestEnv::getRandomDirName() noexcept
{
    size_t rnd = rand() % 100;
    return std::string(CreateTestEnv::folder_names[rnd]);
}

void CreateTestEnv::genDirTree(sfp path, float empty_chance, size_t sub_dir_max)
{
    bool empty = static_cast<float>(rand() % 100) / 100 < empty_chance;
    for (int i = 0; i < rand() % 10; i++)
    {
        std::fstream file(path / (std::to_string(rand()) + ".dat"), std::ios::out);
        file << "data";
        file.close();
    }

    if (empty || sub_dir_max == 0)
        return;


    for (int i = 0; i < rand() % sub_dir_max; i++)
    {
        std::string dir_name = CreateTestEnv::getRandomDirName();
        sfp path_dir_name(path/dir_name);
        try
        {
            std::filesystem::create_directory(path_dir_name);
        }
        catch (const std::exception &e)
        {
            fprintf(stderr, "Error while creating subdirectory %s.\n", (path_dir_name).string().c_str());
            fprintf(stderr, "%s\n", e.what());
            continue;
        }

        try
        {
            CreateTestEnv::genDirTree(
                path_dir_name,
                empty_chance + (empty_chance * CreateTestEnv::empty_chance_increase_ratio),
                --sub_dir_max
            );
        }
        catch (const std::exception &e)
        {
            fprintf(stderr, "Error while creating subdirectories for %s.\n", (path_dir_name).string().c_str());
            fprintf(stderr, "%s\n", e.what());
        }
    }
}

void CreateTestEnv::createEnv(sfp path, float empty_chance, size_t sub_dir_max) noexcept
{
    CreateTestEnv::env_path = path;
    try
    {
        CreateTestEnv::start();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "Error during the startup of CreateTestEnv.\n");
        fprintf(stderr, "%s\n", e.what());
        return;
    }

    try
    {
        CreateTestEnv::genDirTree(path, empty_chance, sub_dir_max);
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "Error during directory tree.\n");
        fprintf(stderr, "%s\n", e.what());
        return;
    }
    


    try
    {
        CreateTestEnv::stop();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "Error during the shutdown of CreateTestEnv.\n");
        fprintf(stderr, "%s\n", e.what());
        return;
    }
}