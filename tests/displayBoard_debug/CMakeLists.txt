cmake_minimum_required(VERSION 3.11)


# your files here
add_executable(displayBoard_debug
    displayBoard.cpp
    "${CMAKE_SOURCE_DIR}/server/src/board.cpp"
    "${CMAKE_SOURCE_DIR}/client-cpp/src/boardscene.cpp"
    )
target_include_directories(displayBoard_debug PRIVATE "${CMAKE_SOURCE_DIR}/server/src")
target_include_directories(displayBoard_debug PRIVATE "${CMAKE_SOURCE_DIR}/client-cpp/src")
target_link_libraries(displayBoard_debug PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
target_link_libraries(displayBoard_debug PRIVATE libhexchess)