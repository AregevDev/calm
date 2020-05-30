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

        external/ImGui/imgui.cpp
        external/ImGui/imgui_demo.cpp
        external/ImGui/imgui_draw.cpp
        external/ImGui/imgui_widgets.cpp
        external/ImGui/imgui_impl_dx11.cpp
        external/ImGui/imgui_impl_win32.cpp
        )

target_include_directories(calm PRIVATE ${SHADERS})
target_include_directories(calm PRIVATE external)
target_link_libraries(calm D3d11 DXGI D3dcompiler)
