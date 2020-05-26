//
// Created by AregevDev on 5/26/2020.
//

#include "app.h"

#include <iostream>

calm::App::App(uint32_t width, uint32_t height, std::string_view title) :
    m_instance(nullptr),
    m_window(nullptr),
    m_width(width),
    m_height(height),
    m_title(title),
    m_running(false),
    m_delta_time(0.0f)
{
    init_application();
}

void calm::App::start()
{
    // Message loop
    MSG msg = {};
    while (msg.message != WM_QUIT && m_running)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            handle_message(msg);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {

        }
    }
}

HWND calm::App::get_hwnd()
{
    return m_window;
}

HINSTANCE calm::App::get_instance()
{
    return m_instance;
}

void calm::App::init_application()
{
    // Register window class
    m_instance = GetModuleHandle(nullptr);

    WNDCLASSEX win_class = {};
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_OWNDC;
    win_class.lpfnWndProc = DefWindowProc;
    win_class.hInstance = m_instance;
    win_class.lpszClassName = m_title.c_str();

    if (!RegisterClassEx(&win_class))
        std::cerr << "Failed to register window class: " << GetLastError() << std::endl;

    // Create window
    m_window = CreateWindowEx(
        0,
        m_title.c_str(),
        m_title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        (uint32_t) m_width,
        (uint32_t) m_height,
        nullptr,
        nullptr,
        m_instance,
        nullptr
        );

    if (!m_window)
        std::cerr << "Failed to create a window: "  << GetLastError() << '\n';

    ShowWindow(m_window, SW_SHOW);
}

void calm::App::handle_message(const MSG& msg)
{
    switch (msg.message)
    {

    }
}
