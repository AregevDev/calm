//
// Created by AregevDev on 5/26/2020.
//

#pragma once

#include "../util/calmwin.h"

#include "device.h"

#include <vector>

namespace calm
{
    class Backend
    {
    public:
        Backend();

        Device create_device(HWND hwnd, size_t adapter_idx);

    private:
        std::vector<IDXGIAdapter*> m_adapters;
    };
}
