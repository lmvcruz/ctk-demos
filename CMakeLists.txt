cmake_minimum_required(VERSION 3.0.0)

project(CTK VERSION 0.1)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 17)
endif ()

add_subdirectory (mosaic/demo-regular)
