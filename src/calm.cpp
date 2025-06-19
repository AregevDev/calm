//
// Created by AregevDev on 19/06/2025.
//

#include "calm/calm.h"

#include <print>

#include <SDL3/SDL.h>
#include <glm/detail/setup.hpp>

int calmGetVersionMajor() {
    return 0;
}

int calmGetVersionMinor() {
    return 0;
}

int calmGetVersionPatch() {
    return 0;
}

void calmInit() {
    SDL_Init(SDL_INIT_VIDEO);
}

void calmPrintVersion() {
    std::println("Calm version: {}.{}.{}",
                 calmGetVersionMajor(),
                 calmGetVersionMinor(),
                 calmGetVersionPatch());

    const int sdlVersion = SDL_GetVersion();
    std::println("Using SDL3 version: {}.{}.{}",
                 SDL_VERSIONNUM_MAJOR(sdlVersion),
                 SDL_VERSIONNUM_MINOR(sdlVersion),
                 SDL_VERSIONNUM_MICRO(sdlVersion));
    
    std::println("Using GLM version: {}.{}.{}", GLM_VERSION_MAJOR, GLM_VERSION_MINOR, GLM_VERSION_PATCH);
}

void calmQuit() {
    SDL_Quit();
}
