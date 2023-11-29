#pragma once

#include <cstdio>
#include <cstdlib> // rand
#include <ctime> // time
#include <string>
#include <fstream>
#include <filesystem>

class CreateTestEnv{
    typedef std::filesystem::path sfp;
    static constexpr float empty_chance_increase_ratio = 0.05;
    static const char * folder_names[];

    static sfp current_path;
    static sfp env_path;

    static void start();
    static void stop();
    static std::string getRandomDirName() noexcept;
    static void genDirTree(sfp path, float empty_chance, size_t sub_dir_max);

public:
    static void createEnv(sfp path, float empty_chance = 0.8, size_t sub_dir_max = 10) noexcept;
};