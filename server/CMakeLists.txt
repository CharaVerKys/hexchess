cmake_minimum_required(VERSION 3.11)

set( log_cpp_file "${CMAKE_SOURCE_DIR}/somecode/log/write.cpp")

add_executable(server 
    ${log_cpp_file} 
    "src/main.cpp"
) # source list

target_include_directories(server PRIVATE "src")