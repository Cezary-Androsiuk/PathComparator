#include <cstdio>

#include "CreateTestEnv.hpp"

int main(int argc, char **argv){
    printf("Second argument runs CreateTestEnv and is used as a path\n");
    printf("Two arguments compares two directories\n");
    if(argc > 1){
        CreateTestEnv::createEnv(argv[1]);
    }


    
}