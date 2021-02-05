cmake_minimum_required(VERSION 3.13)
project(Faz)

# Steps to include gflags in code
add_subdirectory(gflags)
add_executable(main main.cpp)
target_link_libraries(main gflags::gflags)