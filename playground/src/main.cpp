#include <iostream>

#include <SDL3/SDL.h>
#include <glad/glad.h>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Couldn't initialize SDL.\n";
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("calm", 500, 500, SDL_WINDOW_OPENGL);

    SDL_GLContextState *const context = SDL_GL_CreateContext(window);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    if (!gladLoadGL()) {
        std::cerr << "Couldn't initialize glad.\n";
    }

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                quit = true;
            }

            glClearColor(0.14118f, 0.76471f, 0.58039f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_GL_SwapWindow(window);
        }
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
