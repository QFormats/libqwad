add_library(${PROJECT_NAME} SHARED
        palette.cpp
        wad.cpp
        wrapper.cpp
        texture.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "../include/")
