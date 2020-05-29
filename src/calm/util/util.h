//
// Created by AregevDev on 5/26/2020.
//

#pragma once

#include "calmwin.h"

#include <cstdint>
#include <exception>
#include <iostream>

struct Size2D
{
    int32_t x;
    int32_t y;

    Size2D(int32_t x, int32_t y) : x(x), y(y)
    {}
};

void ErrorDescription(HRESULT hr)
{
    if(FACILITY_WINDOWS == HRESULT_FACILITY(hr))
        hr = HRESULT_CODE(hr);
    TCHAR* szErrMsg;

    if(FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&szErrMsg, 0, nullptr) != 0)
    {
        std::cout << "Error: " << szErrMsg << std::endl;
        LocalFree(szErrMsg);
    } else
        std::cout << "Unknown error" << std::endl;
}

void throw_if_failed(HRESULT hr)
{
    if (FAILED(hr))
        ErrorDescription(hr);
}
