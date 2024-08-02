add_library(${PROJECT_NAME} SHARED
        palette.cpp
        wad.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "../include/")
