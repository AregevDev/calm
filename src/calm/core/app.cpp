//
// Created by AregevDev on 5/26/2020.
//

#include "app.h"

#include <ImGui/imgui.h>

#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch (msg)
    {

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}


calm::App::App(uint32_t width, uint32_t height, std::string_view title) :
    m_instance(nullptr),
    m_window(nullptr),
    m_width(width),
    m_height(height),
    m_title(title),
    m_running(true),
    m_delta_time(0.0f)
{
    init_application();
}

calm::App::~App()
{
    DestroyWindow(m_window);
    UnregisterClass(m_title.c_str(), m_instance);
}

int32_t calm::App::start()
{
    return 0;
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
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = window_proc;
    win_class.hInstance = m_instance;
    win_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
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
    m_running = true;
}
