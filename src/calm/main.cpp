#include "util/util.h"

#include "triangle_vs.h"
#include "triangle_ps.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/pbrmaterial.h"
#include "assimp/postprocess.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>

#include <array>
#include <vector>

// TODO: Significant error handling

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 tex_coord;
    XMFLOAT3 tangent;
    XMFLOAT3 bitangent;
};

struct alignas(16) Material
{
   XMFLOAT3 diffuse;
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SIZE:

            break;

        case WM_KEYDOWN:
            break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

size_t load_model(const char* filepath, ID3D11Device* device, ID3D11Buffer** v_buf, ID3D11Buffer** i_buf, ID3D11Buffer** c_buf_mat)
{
    // Load scene
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);
    aiMesh* mesh = scene->mMeshes[0];

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert{};
        vert.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vert.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vert.tex_coord = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        vert.tangent = XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        vert.bitangent = XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

        vertices.push_back(vert);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    // Create the vertex buffer
    D3D11_BUFFER_DESC v_desc = {};
    v_desc.Usage = D3D11_USAGE_DEFAULT;
    v_desc.ByteWidth = vertices.size() * sizeof(Vertex);
    v_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA v_subres = {};
    v_subres.pSysMem = vertices.data();

    throw_if_failed(device->CreateBuffer(&v_desc, &v_subres, v_buf));

    // Create the index buffer
    D3D11_BUFFER_DESC i_desc = {};
    i_desc.Usage = D3D11_USAGE_DEFAULT;
    i_desc.ByteWidth = indices.size() * sizeof(uint32_t);
    i_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA i_subres = {};
    i_subres.pSysMem = indices.data();

    throw_if_failed(device->CreateBuffer(&i_desc, &i_subres, i_buf));

    // Load the materials
    aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

    aiColor3D diffuse(1.0f, 1.0f, 1.0f);
    aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

    Material mat{};
    mat.diffuse = XMFLOAT3(diffuse.r, diffuse.g, diffuse.b);

    D3D11_BUFFER_DESC c_desc = {};
    c_desc.Usage = D3D11_USAGE_DEFAULT;
    c_desc.ByteWidth = sizeof(mat);
    c_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA c_subres = {};
    c_subres.pSysMem = &mat;

    throw_if_failed(device->CreateBuffer(&c_desc, &c_subres, c_buf_mat));

    return indices.size();
}

int main()
{
    uint32_t width = 1280, height = 720;
    std::string title = "calm engine";

    HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASSEX win_class = {};
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = window_proc;
    win_class.hInstance = instance;
    win_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
    win_class.lpszClassName = title.c_str();

    if (!RegisterClassEx(&win_class))
        std::cerr << "Failed to register window class: " << GetLastError() << std::endl;

    // Create window
    HWND window = CreateWindowEx(
        0,
        title.c_str(),
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!window)
        std::cerr << "Failed to create a window: "  << GetLastError() << '\n';

    ShowWindow(window, SW_SHOW);

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swap_chain;

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 3;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.Windowed = true;
    desc.OutputWindow = window;
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
    ds_buffer_desc.Width = width;
    ds_buffer_desc.Height = height;
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
    XMMATRIX model = XMMatrixScaling(5.0f, 5.0f, 5.0f);
    XMMATRIX view = XMMatrixLookAtLH(
        XMVectorSet(9.0f, 4.0f, -9.0f, 1.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)
        );
    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float) width / (float) height,
        0.001, 100.0f
        );

    XMMATRIX mvp = model * view * projection;

    // Vertex buffer
    /*D3D11_BUFFER_DESC buf_desc = {};
    buf_desc.Usage = D3D11_USAGE_DEFAULT;
    buf_desc.ByteWidth = sizeof(VertexPosColor) * vertices.size();
    buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA buf_data = {};
    buf_data.pSysMem = vertices.data();

    uint32_t stride = sizeof(VertexPosColor);
    uint32_t offset = 0;

    throw_if_failed(device->CreateBuffer(&buf_desc, &buf_data, v_buf.GetAddressOf()));*/

    // Load model
    ComPtr<ID3D11Buffer> v_buf, i_buf, c_buf_mat;
    size_t to_draw = load_model("assets/models/icosphere.gltf", device.Get(), &v_buf, &i_buf, &c_buf_mat);

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
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEX_COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    ComPtr<ID3D11InputLayout> input_layout;
    throw_if_failed(device->CreateInputLayout(layout, 5, g_vs_main, sizeof(g_vs_main), input_layout.GetAddressOf()));

    ComPtr<ID3D11RasterizerState> raster;

    D3D11_RASTERIZER_DESC raster_desc = {};
    raster_desc.FillMode = D3D11_FILL_SOLID;
    raster_desc.CullMode = D3D11_CULL_NONE;

    throw_if_failed(device->CreateRasterizerState(&raster_desc, &raster));

    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;
    context->IASetVertexBuffers(0, 1, v_buf.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(i_buf.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetInputLayout(input_layout.Get());

    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = width;
    vp.Height = height;
    vp.MaxDepth = 1.0f;

    context->VSSetShader(vs.Get(), nullptr, 0);
    context->VSSetConstantBuffers(0, 1, cam_buf.GetAddressOf());
    context->RSSetState(raster.Get());
    context->PSSetConstantBuffers(0, 1, c_buf_mat.GetAddressOf());
    context->PSSetShader(ps.Get(), nullptr, 0);
    context->OMSetRenderTargets(1, rtv.GetAddressOf(), ds_view.Get());
    context->OMSetDepthStencilState(ds.Get(), 1);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 20.0f);

    ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device.Get(), context.Get());

    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if ((PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        model *= XMMatrixRotationY(XMConvertToRadians(1.0f));
        mvp = model * view * projection;
        context->UpdateSubresource(cam_buf.Get(), 0, nullptr, &mvp, 0, 0);

        float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
        context->ClearRenderTargetView(rtv.Get(), color);
        context->ClearDepthStencilView(ds_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->RSSetViewports(1, &vp);

        context->DrawIndexed(to_draw, 0, 0);

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swap_chain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(window);
    UnregisterClass(title.c_str(), instance);

    return msg.wParam;
}
