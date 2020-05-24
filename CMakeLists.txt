cmake_minimum_required(VERSION 3.16)
project(calm)

add_subdirectory(external)

set(CMAKE_CXX_STANDARD 17)

add_executable(calm src/calm/main.cpp)
target_include_directories(calm PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(calm ${SDL2_LIBRARIES})
