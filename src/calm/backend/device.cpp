//
// Created by AregevDev on 5/26/2020.
//

#include "device.h"

calm::Device::Device(HWND hwnd, int32_t adapter_idx) :
    m_window(hwnd),
    m_device(nullptr),
    m_swap(nullptr)
{

}

ID3D11Device* calm::Device::get_device()
{
    return m_device.Get();
}

IDXGISwapChain* calm::Device::get_swap_chain()
{
    return m_swap.Get();
}
