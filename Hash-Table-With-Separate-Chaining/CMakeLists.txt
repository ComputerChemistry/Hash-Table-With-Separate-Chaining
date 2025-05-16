cmake_minimum_required(VERSION 3.13)
project(hash_table_with_separate_chaining)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES "Sources/*.cpp")
file(GLOB HEADER_FILES "Headers/*.hpp")

add_executable(hash_table_with_separate_chaining
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

target_include_directories(hash_table_with_separate_chaining PRIVATE
        Headers
        Templates
)
