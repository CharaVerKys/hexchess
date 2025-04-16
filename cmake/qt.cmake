set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(RES_FILE resources/resources.qrc)
# set(CMAKE_GLOBAL_AUTORCC_TARGET ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# target_link_libraries(${PROJECT_NAME} PUBLIC
#     ${CMAKE_THREAD_LIBS_INIT}
#     Qt${QT_VERSION_MAJOR}::Core
#     Qt${QT_VERSION_MAJOR}::Gui
#     Qt${QT_VERSION_MAJOR}::Widgets
# )