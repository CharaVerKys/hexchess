set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions( -DRELEASE )
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # nothing
else()
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Please use Debug or Release.")
endif() 