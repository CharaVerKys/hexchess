cmake_minimum_required(VERSION 3.11)

set(test_name test_board_getColumn)

# your files here
add_executable(${test_name}
    ${test_name}.cpp
    # "${CMAKE_SOURCE_DIR}/server/src/board.cpp"
    )
target_include_directories(${test_name} PRIVATE "${CMAKE_SOURCE_DIR}/server/src")

add_test(NAME ${test_name} COMMAND ${test_name})