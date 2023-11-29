#pragma once

#include <cstdio>
#include <cstdlib> // rand
#include <ctime> // time
#include <string>
#include <filesystem>

class CreateTestEnv{
    typedef std::filesystem::path sfp;
    static const char * folderNames[];

    static sfp current_path;
    static sfp env_path;

    static void start();
    static void stop();

public:
    static void createEnv(sfp path);
};