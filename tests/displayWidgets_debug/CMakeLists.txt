cmake_minimum_required(VERSION 3.11)


# your files here
add_executable(displayWidgets_debug
    display.cpp
    "${CMAKE_SOURCE_DIR}/client-cpp/src/matchwidget.cpp"
    "${CMAKE_SOURCE_DIR}/client-cpp/src/matchlistwidget.cpp"
    )
target_include_directories(displayWidgets_debug PRIVATE "${CMAKE_SOURCE_DIR}/client-cpp/src")
target_link_libraries(displayWidgets_debug PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
target_link_libraries(displayWidgets_debug PRIVATE libhexchess)