#include "CreateTestEnv.hpp"

const char *CreateTestEnv::folderNames[] = {
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
    "Challenges", "Reflections", "Dreams", "Ideas"};

void CreateTestEnv::start()
{
    CreateTestEnv::current_path = std::filesystem::current_path();
    if (std::filesystem::exists(CreateTestEnv::env_path))
    {
        
    }
    else
    {


    }
}

void CreateTestEnv::stop()
{
    std::filesystem::current_path(CreateTestEnv::current_path);
}

void CreateTestEnv::createEnv(sfp path)
{
    CreateTestEnv::env_path = path;
    try
    {
        CreateTestEnv::start();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "Error during the startup of CreateTestEnv.");
        return;
    }

    try
    {
        CreateTestEnv::stop();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "Error during the shutdown of CreateTestEnv.");
        return;
    }
}