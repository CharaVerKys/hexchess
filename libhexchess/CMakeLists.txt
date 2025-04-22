
add_library(libhexchess STATIC
    field_ranges.cpp
    packetsocketoperations.cpp
    protocolallboardpeaces.cpp
)

target_include_directories(libhexchess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) # public for includes, i guess
