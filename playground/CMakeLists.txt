add_executable(calm_playground src/main.cpp)
target_link_libraries(calm_playground PRIVATE calm)

b_embed(calm_playground shaders/triangle.vert)
b_embed(calm_playground shaders/triangle.frag)