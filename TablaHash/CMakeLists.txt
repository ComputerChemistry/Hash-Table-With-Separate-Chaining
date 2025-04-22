cmake_minimum_required(VERSION 3.30)
project(TablaHash)

set(CMAKE_CXX_STANDARD 20)

add_executable(TablaHash main.cpp
        TablaHash.cpp
        TablaHash.hpp)
