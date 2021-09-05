#==========================
#to use, type: cmake .
#==========================
project(circlePack)                                  # the project name
cmake_minimum_required (VERSION 3.9)
set (CMAKE_CXX_STANDARD 11)
include_directories (include)
#================
#include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
#================
set (TARGET circlePack)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
add_executable (${TARGET} src/main.cpp src/atom.cpp src/dem.cpp src/params.cpp)
