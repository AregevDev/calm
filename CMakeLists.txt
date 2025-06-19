cmake_minimum_required(VERSION 3.15)
project(calm)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ext)

set (CALM_SOURCE_FILES
        src/calm.cpp
)

add_library(calm ${CALM_SOURCE_FILES})
target_include_directories(calm PUBLIC include PUBLIC ext/SDL/include ext/glad)
target_link_libraries(calm PRIVATE glm::glm SDL3::SDL3-static glad)

add_subdirectory(playground)
