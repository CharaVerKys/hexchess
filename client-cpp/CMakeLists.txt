cmake_minimum_required(VERSION 3.11)

add_executable(client-cpp
    "src/boardscene.cpp"
    "src/main.cpp"
    "src/asio.cpp"
    "src/interfaceasio.cpp"
    "src/matchwidget.cpp"
    "src/matchlistwidget.cpp"
    "src/controlwindow.cpp"
    "src/clientcontroller.cpp"
    "src/buttonswidget.cpp"
    )
target_link_libraries(client-cpp PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
target_link_libraries(client-cpp PRIVATE libhexchess)
target_include_directories(client-cpp PRIVATE "${CMAKE_SOURCE_DIR}/client-cpp/src")
target_compile_definitions(client-cpp PRIVATE -DUSE_CHECK_THREAD_ASIO_IN_SWITCH_CONTEXT)
target_compile_definitions(client-cpp PRIVATE -DCHECK_THREAD_HEADER=<threadcheck.hpp>)