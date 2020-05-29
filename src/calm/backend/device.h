//
// Created by AregevDev on 5/26/2020.
//

#pragma once

#include "../util/calmwin.h"

#include <cstdint>

namespace calm
{
    class Device
    {
    public:
        explicit Device(HWND hwnd, int32_t adapter);

        ID3D11Device* get_device();
        IDXGISwapChain* get_swap_chain();

    private:
        HWND m_window;
        int32_t adapter_idx;

        ComPtr<ID3D11Device> m_device;
        ComPtr<IDXGISwapChain> m_swap;

        void get_adapters();
    };
}
