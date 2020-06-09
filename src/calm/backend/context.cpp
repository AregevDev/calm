//
// Created by AregevDev on 6/9/2020.
//

#include "context.h"

#include <spdlog/include/spdlog/spdlog.h>

#include <iostream>
#include <vector>

void ErrorDescription(HRESULT hr);

void throw_if_failed(HRESULT hr);

Context::Context(HWND hwnd, int32_t adapter_index)
{
    spdlog::info("Initializing calm engine");

    ComPtr<IDXGIFactory> factory;
    throw_if_failed(CreateDXGIFactory(IID_PPV_ARGS(&factory)));

    ComPtr<IDXGIAdapter> adapter;
    std::vector<ComPtr<IDXGIAdapter>> adapters;

    for (uint32_t i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
    {
        adapters.push_back(adapter);
    }

    spdlog::info("Found {} compatible adapters:", adapters.size());

    for (ComPtr<IDXGIAdapter>& a : adapters)
    {
        DXGI_ADAPTER_DESC desc;
        a->GetDesc(&desc);

        std::wstring s = desc.Description;
        std::string d(s.length(), '\0');
        std::transform(s.begin(), s.end(), d.begin(), [](wchar_t ch){ return CHAR(ch); });

        spdlog::info("\t" + d);
    }

    if (adapter_index >= adapters.size())
    {
        spdlog::error("Invalid adapter index");
        exit(1);
    }

    D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL level_result;

    HRESULT hr = D3D11CreateDevice(
        adapters[adapter_index].Get(),
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        &level,
        1,
        D3D11_SDK_VERSION,
        &m_device,
        &level_result,
        &m_context
    );

    throw_if_failed(hr);

    if (FAILED(hr))
        spdlog::error("Failed to create a device");
    else
        spdlog::info("Device created successfully");

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 3;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.Windowed = true;
    desc.OutputWindow = hwnd;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    throw_if_failed(factory->CreateSwapChain(m_device.Get(), &desc, &m_swap_chain));
}
