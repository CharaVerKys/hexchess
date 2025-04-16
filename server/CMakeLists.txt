cmake_minimum_required(VERSION 3.11)

add_executable(server "src/main.cpp")

target_include_directories(server PRIVATE "src")