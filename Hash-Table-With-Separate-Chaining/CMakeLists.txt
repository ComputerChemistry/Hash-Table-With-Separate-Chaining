cmake_minimum_required(VERSION 3.13)  # CMake version check
project(hash_table_with_separate_chaining)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

file(GLOB SOURCE_FILES "Sources/*.cpp")
file(GLOB HEADER_FILES "Headers/*.hpp")
file(GLOB TEMPLATE_FILES "Templates/.tpp*")

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(hash_table_with_separate_chaining ${SOURCE_FILES})




