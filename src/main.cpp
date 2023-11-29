#include <cstdio>

#include "CreateTestEnv.hpp"
#include "PathComparator.hpp"

int main(int argc, char **argv){
    if(argc > 1){
        printf("CreateTestEnv:\n");
        CreateTestEnv::createEnv(argv[1]);
    }
    else if(argc > 2){
        printf("PathComparator:\n");
        PathComparator::compareStructures(argv[1], argv[2]);
    }
    else{
        printf("\nSecond argument runs CreateTestEnv and is used as a path\n");
        printf("Two arguments compares two directories\n\n");
    }
}