//
// Created by AregevDev on 5/24/2020.
//

#pragma once

#include "../calm_win.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <string>
#include <memory>

namespace calm
{
    class Window
    {
    public:
        int width;
        int height;
        std::string title;

        Window(int width, int height, std::string_view title);
        ~Window();

        [[nodiscard]] bool should_close() const noexcept;
        void close() noexcept;

        [[nodiscard]] SDL_Window* get_window() const noexcept;
        [[nodiscard]] HWND get_hwnd() const noexcept;
        [[nodiscard]] HINSTANCE get_hinstance() const noexcept;

    private:
        bool _close;

        SDL_Window *_window;
        HWND _hwnd;
        HINSTANCE _hinstance;
    };
}
