//
// Created by AregevDev on 5/26/2020.
//

#pragma once

#include "../util/calmdef.h"
#include "../util/calmwin.h"

#include <string>
#include <string_view>

namespace calm
{
    class App
    {
    public:
        App(u32 width, u32 height, std::string_view title);
        ~App() = default;

        void start();

        HWND get_hwnd();
        HINSTANCE get_instance();

    private:
        HINSTANCE m_instance;
        HWND m_window;

        u32 m_width;
        u32 m_height;
        std::string m_title;

        bool m_running;
        f32 m_delta_time;

        void init_application();
        void handle_message(const MSG& msg);
    };
}
