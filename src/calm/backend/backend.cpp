//
// Created by AregevDev on 5/26/2020.
//

#include "backend.h"

#include <iostream>

calm::Backend::Backend()
{
    IDXGIFactory1* factory = nullptr;
    CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&factory));

    IDXGIAdapter1* adapter = nullptr;
    for (uint32_t i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
        m_adapters.push_back(adapter);

    factory->Release();
}

calm::Device calm::Backend::create_device(HWND hwnd, size_t adapter_idx)
{
    return Device(hwnd, adapter_idx);
}
