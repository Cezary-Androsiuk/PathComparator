#include <cstdio>

#include "CreateTestEnv.hpp"
#include "PathComparator.hpp"

// BUILD LINUX
// mkdir bin && cd bin;
// cmake .. && make && ./main;

// BUILD WINDOWS
// mkdir bin && cd bin;
// cmake -G "MinGW Makefiles" .. && cmake --build . && .\main

int main(int argc, char **argv)
{
    if (argc > 3)
    {
        printf("CreateTestEnv:\n");
        CreateTestEnv::createEnv(argv[1]);
    }
    else if (argc > 2)
    {
        printf("PathComparator:\n");
        PathComparator::compareStructures(argv[1], argv[2]);
    }
    else if (argc > 1)
    {
        PathComparator::printStructure(argv[1]);
    }
    else
    {
        printf("one argument: prints structure of directory/file containing structure\n");
        printf("two arguments [old] [new]: comparing structures\n");
        printf("tree arguments [path] [?] [?]: creates test environment in path\n");
    }
}