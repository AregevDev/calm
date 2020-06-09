//
// Created by AregevDev on 6/9/2020.
//

#pragma once

#include "../util/calmwin.h"

class Context
{
public:
    ComPtr<IDXGIAdapter> m_adapter;
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swap_chain;

    explicit Context(HWND hwnd, int32_t adapter_index = 0);
};
