cmake_minimum_required(VERSION 3.11)
project(random-mosaic VERSION 0.1.0)

set(WORKSPACE ${PROJECT_SOURCE_DIR}/../../../)

add_executable(random-mosaics
    random.cpp
)

find_package(OpenCV REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/../src
    ${WORKSPACE}/ctk
    ${WORKSPACE}/ext/argparse/include
)

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${WORKSPACE}/ctk/build/ctk/libctklib.a)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})