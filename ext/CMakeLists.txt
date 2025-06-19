add_library(glad glad/glad.c)
target_include_directories(glad PUBLIC glad)

set(SDL_SHARED ON)
set(SDL_STATIC ON)
add_subdirectory(SDL)
add_subdirectory(glm)
