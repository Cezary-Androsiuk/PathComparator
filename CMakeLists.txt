cmake_minimum_required(VERSION 3.10)

project(SFML_Schedule_visualizer VERSION 0.1.0)

# make variables with paths
set(INC_DIR             ${CMAKE_SOURCE_DIR}/inc)
set(SRC_DIR             ${CMAKE_SOURCE_DIR}/src)
set(RES_DIR             ${CMAKE_SOURCE_DIR}/res)

# include headers to project
include_directories(${INC_DIR})

# compile each single source file
file(GLOB SOURCES ${SRC_DIR}/*.cpp ${SRC_DIR}/*.rc)

# copy resource files to bin/ 
file(COPY ${RES_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# link compiled files to main
add_executable(main ${SOURCES})
