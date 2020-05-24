//
// Created by AregevDev on 5/24/2020.
//

#include "device.h"

#include <iostream>
#include <system_error>

calm::Device::Device(HWND hwnd) : _hwnd(hwnd)
{
    _create_device();
    _create_swap_chain();
}

void calm::Device::present()
{
    _swap->Present(1, 0);
}

void calm::Device::_create_device()
{
    // Device creation flags
    uint32_t flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifndef NDEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL fl_requested = D3D_FEATURE_LEVEL_11_0;
    int32_t fl_num = 1;
    D3D_FEATURE_LEVEL fl_supported;

    ComPtr<ID3D11Device> device_temp;
    ComPtr<ID3D11DeviceContext> ctx_temp;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        &fl_requested,
        fl_num,
        D3D11_SDK_VERSION,
        &device_temp,
        &fl_supported,
        &ctx_temp
    );

    if (hr == S_OK)
    {
        device_temp.As(&_device);
        ctx_temp.As(&_ctx);

        std::cout << "Device Created Successfully" << '\n';
    }
    else
        std::cout << std::system_category().message(hr) << '\n';
}

void calm::Device::_create_swap_chain()
{
    // Swap Chain description
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.SampleDesc.Count = 1;

    // Access DXGI through the device
    ComPtr<IDXGIDevice1> dxgi_device;
    _device.As(&dxgi_device);

    ComPtr<IDXGIAdapter> dxgi_adapter;
    dxgi_device->GetAdapter(&dxgi_adapter);

    ComPtr<IDXGIFactory2> dxgi_factory;
    dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), &dxgi_factory);

    // Create the swap chain using the factory
    HRESULT hr = dxgi_factory->CreateSwapChainForHwnd(
        dxgi_device.Get(),
        _hwnd,
        &desc,
        nullptr,
        nullptr,
        &_swap
        );

    if (hr == S_OK)
        std::cout << "Swap Chain created successfully" << '\n';
    else
        std::cout << std::system_category().message(hr) << '\n';
}
