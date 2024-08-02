cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 20)

project(
        qwad
        VERSION 1.0.0
        LANGUAGES CXX
)

add_subdirectory(src)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")