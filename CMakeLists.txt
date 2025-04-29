cmake_minimum_required(VERSION 3.15)

project(Hash_Table_With_Separate_Chaining)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Hash_Table_With_Separate_Chaining
        main.cpp
        AlgoritmoHashing/xxh64.hpp
)

target_include_directories(Hash_Table_With_Separate_Chaining PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/AlgoritmoHashing
)