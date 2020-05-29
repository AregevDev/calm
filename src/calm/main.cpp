#include "core/app.h"
#include "util/util.h"
#include "util/calmwin.h"

#include "triangle_vs.h"
#include "triangle_ps.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>

#include <array>

// TODO: Significant error handling

struct VertexPosColor
{
    XMFLOAT3 position;
    XMFLOAT3 color;
};

int main()
{
    calm::App app(1280, 720, "calm engine");

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swap_chain;

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.Windowed = true;
    desc.OutputWindow = app.get_hwnd();
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL supported;

    throw_if_failed(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        &level,
        1,
        D3D11_SDK_VERSION,
        &desc,
        &swap_chain,
        &device,
        &supported,
        &context
    ));

    ComPtr<ID3D11Texture2D> back_buffer;
    throw_if_failed(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer));

    ComPtr<ID3D11RenderTargetView> rtv;
    throw_if_failed(device->CreateRenderTargetView(back_buffer.Get(), nullptr, rtv.GetAddressOf()));
    context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);

    std::array<VertexPosColor, 3> vertices = {{
        {XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
        {XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
        }};

    // Vertex buffer
    ComPtr<ID3D11Buffer> v_buf;

    D3D11_BUFFER_DESC buf_desc = {};
    buf_desc.Usage = D3D11_USAGE_DEFAULT;
    buf_desc.ByteWidth = sizeof(VertexPosColor) * vertices.size();
    buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA buf_data = {};
    buf_data.pSysMem = vertices.data();
    buf_data.SysMemPitch = 0;
    buf_data.SysMemSlicePitch = 0;

    uint32_t stride = sizeof(VertexPosColor);
    uint32_t offset = 0;

    throw_if_failed(device->CreateBuffer(&buf_desc, &buf_data, v_buf.GetAddressOf()));

    // Vertex shader
    ComPtr<ID3D11VertexShader> vs = nullptr;
    throw_if_failed(device->CreateVertexShader(g_vs_main, sizeof(g_vs_main), nullptr, vs.GetAddressOf()));

    // Pixel shader
    ComPtr<ID3D11PixelShader> ps = nullptr;
    throw_if_failed(device->CreatePixelShader(g_ps_main, sizeof(g_ps_main), nullptr, ps.GetAddressOf()));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    ComPtr<ID3D11InputLayout> input_layout;
    throw_if_failed(device->CreateInputLayout(layout, 2, g_vs_main, sizeof(g_vs_main), input_layout.GetAddressOf()));
    context->IASetVertexBuffers(0, 1, v_buf.GetAddressOf(), &stride, &offset);

    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = app.m_width;
    vp.Height = app.m_height;
    vp.MaxDepth = 1.0f;

    context->IASetInputLayout(input_layout.Get());
    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 20.0f);

    ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(app.get_hwnd());
    ImGui_ImplDX11_Init(device.Get(), context.Get());

    MSG msg = {};
    while ((GetMessage(&msg, nullptr, 0, 0)) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
        context->ClearRenderTargetView(rtv.Get(), color);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->RSSetViewports(1, &vp);

        context->Draw(3, 0);

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swap_chain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    return msg.wParam;
}
