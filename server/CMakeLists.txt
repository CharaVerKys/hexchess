cmake_minimum_required(VERSION 3.11)

set( log_cpp_file "${CMAKE_SOURCE_DIR}/somecode/log/write.cpp")

add_executable(server 
    ${log_cpp_file} 
    "src/main.cpp"
    "src/board.cpp"
    "src/movement.cpp"
    "src/idpool.cpp"
    "src/sessionscontrol.cpp"
    "src/matchcontrol.cpp"
) # source list

target_include_directories(server PRIVATE "src")
target_link_libraries(server PRIVATE libhexchess)
target_link_libraries(server PRIVATE SQLiteCpp)