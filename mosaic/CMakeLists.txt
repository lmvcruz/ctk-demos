cmake_minimum_required(VERSION 3.11)
project(mosaic VERSION 0.1.0)

add_executable(mosaic
    main.cpp
)

find_package(OpenCV REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/../../ctk
    ${PROJECT_SOURCE_DIR}/../../ext/argparse/include
)

target_link_libraries(${PROJECT_NAME} 
    ${OpenCV_LIBS}
    ${PROJECT_SOURCE_DIR}/../../ctk/build/ctk/libctklib.a)