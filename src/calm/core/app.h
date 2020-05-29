//
// Created by AregevDev on 5/26/2020.
//

#pragma once

#include "../util/calmwin.h"

#include <string>
#include <string_view>

namespace calm
{
    class App
    {
    public:
        uint32_t m_width;
        uint32_t m_height;

        App(uint32_t width, uint32_t height, std::string_view title);
        ~App() = default;

        int32_t start();

        HWND get_hwnd();
        HINSTANCE get_instance();

    private:
        HINSTANCE m_instance;
        HWND m_window;

        std::string m_title;

        bool m_running;
        float m_delta_time;

        void init_application();
    };
}
