cmake_minimum_required(VERSION 3.11)

enable_testing()
message("start configuring tests")

add_subdirectory(test_board_getColumn)

add_subdirectory(displayBoard_debug)

message("tests configured")
