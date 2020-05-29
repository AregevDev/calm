cmake_minimum_required(VERSION 3.16)
project(calm)

add_subdirectory(external)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SHADERS
        assets/shaders/compiled
        )

add_executable(calm
        src/calm/main.cpp

        src/calm/core/app.cpp
        src/calm/backend/backend.cpp
        src/calm/backend/device.cpp
        )

target_include_directories(calm PRIVATE ${SHADERS})
target_link_libraries(calm SDL2-static D3d11 DXGI D3dcompiler )
