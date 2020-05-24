cmake_minimum_required(VERSION 3.16)
project(calm)

add_subdirectory(external)

set(CMAKE_CXX_STANDARD 17)

add_executable(calm
        src/calm/main.cpp
        src/calm/core/window.cpp
        )

target_link_libraries(calm SDL2-static)
