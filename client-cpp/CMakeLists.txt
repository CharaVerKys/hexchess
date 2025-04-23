cmake_minimum_required(VERSION 3.11)

add_executable(client-cpp
    "src/boardscene.cpp"
    "src/main.cpp"
    "src/interfaceasio.cpp"
    "src/asio.cpp"
    )
target_link_libraries(client-cpp PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
target_link_libraries(client-cpp PRIVATE libhexchess)