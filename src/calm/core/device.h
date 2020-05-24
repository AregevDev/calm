//
// Created by AregevDev on 5/24/2020.
//

#pragma once

#include "../calm_win.h"

#include <memory>

namespace calm
{
    class Device
    {
    public:
        explicit Device(HWND hwnd);

        void present();

    private:
        HWND _hwnd;

        ComPtr<ID3D11Device1> _device;
        ComPtr<ID3D11DeviceContext1> _ctx;
        ComPtr<IDXGISwapChain1> _swap;
        ComPtr<ID3D11RenderTargetView> _rtv;

        void _create_device();
        void _create_swap_chain();
    };
}
