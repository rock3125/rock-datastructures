cmake_minimum_required(VERSION 3.26)
project(c_code C)

set(CMAKE_C_STANDARD 11)

add_executable(c_code main.c
        model/string_hash_set.c
        model/string_hash_set.h
)

target_link_libraries(c_code PUBLIC z)
