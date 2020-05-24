//
// Created by AregevDev on 5/24/2020.
//

#include "window.h"

#include "SDL_syswm.h"

calm::Window::Window(int width, int height, std::string_view title) : width(width), height(height), title(title)
{
    // TODO: Figure out what subsystems are needed exactly
    if (!SDL_WasInit(SDL_INIT_EVERYTHING))
        SDL_Init(SDL_INIT_EVERYTHING);

    // Create the window
    _window = SDL_CreateWindow(title.data(),SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,SDL_WINDOW_RESIZABLE);
    _close = false;

    // Retrieve the native Windows handles
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(_window, &wm_info);

    _hwnd = wm_info.info.win.window;
    _hinstance = wm_info.info.win.hinstance;
}

calm::Window::~Window()
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

bool calm::Window::should_close() const noexcept
{
    return _close;
}

void calm::Window::close() noexcept
{
    _close = true;
}

SDL_Window *calm::Window::get_window() const noexcept
{
    return _window;
}

HWND calm::Window::get_hwnd() const noexcept
{
    return _hwnd;
}

HINSTANCE calm::Window::get_hinstance() const noexcept
{
    return _hinstance;
}
