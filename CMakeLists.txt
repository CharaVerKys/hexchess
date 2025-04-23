cmake_minimum_required(VERSION 3.11)

include(cmake/set.cmake)

project(HexChess VERSION 0.1 LANGUAGES CXX)

include(cmake/compile.cmake)
include(cmake/qt.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sqlitecpp)
add_subdirectory(libhexchess)
add_subdirectory(server)
add_subdirectory(client-cpp)
add_subdirectory(tests)