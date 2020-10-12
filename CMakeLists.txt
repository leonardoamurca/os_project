cmake_minimum_required(VERSION 3.17)
project(os_project)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

add_executable(os_project main.cpp)
