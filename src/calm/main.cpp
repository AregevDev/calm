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
    std::cout << app.m_height << '\n';

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

    // Color + Depth + Stencil
    ComPtr<ID3D11Texture2D> tex_col;
    ComPtr<ID3D11RenderTargetView> rtv;
    throw_if_failed(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &tex_col));
    throw_if_failed(device->CreateRenderTargetView(tex_col.Get(), nullptr, rtv.GetAddressOf()));

    ComPtr<ID3D11Texture2D> tex_ds;
    ComPtr<ID3D11DepthStencilState> ds;

    D3D11_TEXTURE2D_DESC ds_buffer_desc = {};
    ds_buffer_desc.Width = app.m_width;
    ds_buffer_desc.Height = app.m_height;
    ds_buffer_desc.MipLevels = 1;
    ds_buffer_desc.ArraySize = 1;
    ds_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds_buffer_desc.SampleDesc.Count = 1;
    ds_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    CD3D11_DEPTH_STENCIL_DESC ds_state_desc;
    ds_state_desc.DepthEnable = true;
    ds_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    ds_state_desc.DepthFunc = D3D11_COMPARISON_LESS;
    ds_state_desc.StencilEnable = true;
    ds_state_desc.StencilReadMask = 0xFF;
    ds_state_desc.StencilWriteMask = 0xFF;

    ds_state_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    ds_state_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    ds_state_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    ds_state_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ds_state_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    ds_state_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    ds_state_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    ds_state_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    throw_if_failed(device->CreateTexture2D(&ds_buffer_desc, nullptr, tex_ds.GetAddressOf()));
    throw_if_failed(device->CreateDepthStencilState(&ds_state_desc, ds.GetAddressOf()));

    ComPtr<ID3D11DepthStencilView> ds_view;
    D3D11_DEPTH_STENCIL_VIEW_DESC ds_view_desc = {};
    ds_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    ds_view_desc.Texture2D.MipSlice = 0;

    throw_if_failed(device->CreateDepthStencilView(tex_ds.Get(), &ds_view_desc, ds_view.GetAddressOf()));

    // Data
    std::array<VertexPosColor, 3> vertices = {{
        {XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
        {XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
        }};

    XMMATRIX model = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(
        XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        );
    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float) app.m_width / (float) app.m_height,
        0.001, 100.0f
        );

    XMMATRIX mvp = model * view * projection;

    // Vertex buffer
    ComPtr<ID3D11Buffer> v_buf;

    D3D11_BUFFER_DESC buf_desc = {};
    buf_desc.Usage = D3D11_USAGE_DEFAULT;
    buf_desc.ByteWidth = sizeof(VertexPosColor) * vertices.size();
    buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA buf_data = {};
    buf_data.pSysMem = vertices.data();

    uint32_t stride = sizeof(VertexPosColor);
    uint32_t offset = 0;

    throw_if_failed(device->CreateBuffer(&buf_desc, &buf_data, v_buf.GetAddressOf()));

    // Constant buffer
    ComPtr<ID3D11Buffer> cam_buf;

    D3D11_BUFFER_DESC cam_buf_desc = {};
    cam_buf_desc.Usage = D3D11_USAGE_DEFAULT;
    cam_buf_desc.ByteWidth = sizeof(mvp);
    cam_buf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA cam_data = {};
    cam_data.pSysMem = &mvp;

    throw_if_failed(device->CreateBuffer(&cam_buf_desc, &cam_data, cam_buf.GetAddressOf()));

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
    context->VSSetConstantBuffers(0, 1, cam_buf.GetAddressOf());
    context->PSSetShader(ps.Get(), nullptr, 0);
    context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
    context->OMSetDepthStencilState(ds.Get(), 1);

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
