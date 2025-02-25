#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

constexpr UINT WINDOW_WIDTH = 800;
constexpr UINT WINDOW_HEIGHT = 600;

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texCoord;
};

struct MatrixBuffer
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX wvp;
};

struct LightBuffer
{
    DirectX::XMFLOAT4 ambientColor;
    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT3 lightDirection;
    float padding;
};

struct CameraState
{
    DirectX::XMFLOAT3 position = {0.0f, 0.0f, -3.0f};
    float yaw = 0.0f;
    float pitch = 0.0f;
    bool isMouseDown = false;
    POINT prevPosition = {0, 0};
};

class Direct3DRenderer
{
public:
    Direct3DRenderer(HINSTANCE instanceHandle);
    ~Direct3DRenderer();

    bool Initialize();
    void Run();

private:
    bool InitializeWindow();
    bool InitializeDirect3D();
    bool CreateD3DDeviceAndSwapChain();
    bool InitializeRasterizerStates();
    bool InitializeRenderTargetAndDepthBuffer();
    bool InitializeTextureAndSampler();
    bool ConfigureViewport();
    bool InitializeTextRenderer();
    bool InitializeShadersAndInputLayout();
    bool InitializeGeometryBuffers();
    bool InitializeLightingBuffer();

    void Update();
    void Render();
    void UpdateTiming();
    void UpdateCubeRotation();
    void UpdateCameraMovement();
    void ClearBuffers();
    void ConfigureCameraMatrices(DirectX::XMMATRIX &view, DirectX::XMMATRIX &projection);
    void DrawCube(const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);
    void RenderHUD();
    void ResetRenderStates();
    bool OnResize(UINT newWidth, UINT newHeight);

    static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    HINSTANCE instanceHandle;
    HWND windowHandle;
    UINT windowWidth = WINDOW_WIDTH;
    UINT windowHeight = WINDOW_HEIGHT;
    bool isWireframeEnabled = false;

    CameraState camera;

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> matrixConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterizerState;

    float cubeRotationAngle = 0.0f;
    std::chrono::high_resolution_clock::time_point lastTime;
    float deltaTime = 0.0f;
    float fpsTimeAccumulator = 0.0f;
    int fpsFrameCount = 0;
    int currentFPS = 0;

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;
};

Direct3DRenderer::Direct3DRenderer(HINSTANCE instanceHandle)
    : instanceHandle(instanceHandle), windowHandle(nullptr)
{
}

Direct3DRenderer::~Direct3DRenderer()
{
    // COM objects are automatically released.
}

bool Direct3DRenderer::Initialize()
{
    if (!InitializeWindow())
        return false;
    lastTime = std::chrono::high_resolution_clock::now();
    if (!InitializeDirect3D())
        return false;
    return true;
}

bool Direct3DRenderer::InitializeWindow()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Direct3DRenderer::WindowProc;
    wc.hInstance = instanceHandle;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"Direct3DRendererWindowClass";
    if (!RegisterClassEx(&wc))
        return false;

    RECT rc = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    windowHandle = CreateWindow(L"Direct3DRendererWindowClass", L"Direct3D 11 Renderer",
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                rc.right - rc.left, rc.bottom - rc.top,
                                nullptr, nullptr, instanceHandle, this);
    if (!windowHandle)
        return false;

    ShowWindow(windowHandle, SW_SHOW);
    return true;
}

bool Direct3DRenderer::InitializeDirect3D()
{
    if (!CreateD3DDeviceAndSwapChain())
        return false;
    if (!InitializeRasterizerStates())
        return false;
    if (!InitializeRenderTargetAndDepthBuffer())
        return false;
    if (!InitializeTextureAndSampler())
        return false;
    if (!ConfigureViewport())
        return false;
    if (!InitializeTextRenderer())
        return false;
    if (!InitializeShadersAndInputLayout())
        return false;
    if (!InitializeGeometryBuffers())
        return false;
    if (!InitializeLightingBuffer())
        return false;
    return true;
}

bool Direct3DRenderer::CreateD3DDeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = WINDOW_WIDTH;
    sd.BufferDesc.Height = WINDOW_HEIGHT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = windowHandle;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, swapChain.GetAddressOf(),
        d3dDevice.GetAddressOf(), nullptr, d3dContext.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Direct3DRenderer::InitializeRasterizerStates()
{
    D3D11_RASTERIZER_DESC rd = {};
    rd.CullMode = D3D11_CULL_BACK;
    rd.FrontCounterClockwise = false;
    rd.DepthClipEnable = TRUE;
    rd.FillMode = D3D11_FILL_SOLID;
    HRESULT hr = d3dDevice->CreateRasterizerState(&rd, solidRasterizerState.GetAddressOf());
    if (FAILED(hr))
        return false;

    rd.FillMode = D3D11_FILL_WIREFRAME;
    hr = d3dDevice->CreateRasterizerState(&rd, wireframeRasterizerState.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Direct3DRenderer::InitializeRenderTargetAndDepthBuffer()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                      reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = WINDOW_WIDTH;
    depthDesc.Height = WINDOW_HEIGHT;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    hr = d3dDevice->CreateTexture2D(&depthDesc, nullptr, depthBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateDepthStencilView(depthBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
    if (FAILED(hr))
        return false;

    d3dContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = FALSE;
    hr = d3dDevice->CreateDepthStencilState(&dsDesc, depthStencilState.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Direct3DRenderer::InitializeTextureAndSampler()
{
    HRESULT hr = DirectX::CreateWICTextureFromFile(d3dDevice.Get(), d3dContext.Get(), L"assets/crying_rat.png", nullptr, textureResourceView.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = d3dDevice->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Direct3DRenderer::ConfigureViewport()
{
    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(WINDOW_WIDTH);
    vp.Height = static_cast<FLOAT>(WINDOW_HEIGHT);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dContext->RSSetViewports(1, &vp);
    return true;
}

bool Direct3DRenderer::InitializeTextRenderer()
{
    try
    {
        spriteBatch = std::make_unique<DirectX::SpriteBatch>(d3dContext.Get());
        spriteFont = std::make_unique<DirectX::SpriteFont>(d3dDevice.Get(), L"assets/Arial.spritefont");
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool Direct3DRenderer::InitializeShadersAndInputLayout()
{
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    HRESULT hr = D3DCompileFromFile(L"shaders/vertexShader.hlsl", nullptr, nullptr,
                                    "main", "vs_5_0", 0, 0, vsBlob.GetAddressOf(), nullptr);
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                       nullptr, vertexShader.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    hr = d3dDevice->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc),
                                      vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                      inputLayout.GetAddressOf());
    if (FAILED(hr))
        return false;
    d3dContext->IASetInputLayout(inputLayout.Get());

    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    hr = D3DCompileFromFile(L"shaders/pixelShader.hlsl", nullptr, nullptr,
                            "main", "ps_5_0", 0, 0, psBlob.GetAddressOf(), nullptr);
    if (FAILED(hr))
        return false;
    hr = d3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                                      nullptr, pixelShader.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Direct3DRenderer::InitializeGeometryBuffers()
{
    Vertex vertices[] = {
        // front face (normal: 0, 0, -1)
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(1, 0, 0, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(0, 1, 0, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(0, 0, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // back face (normal: 0, 0, 1)
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(1, 1, 0, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(0, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(1, 0, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(0, 0, 0, 1), DirectX::XMFLOAT2(0, 1)},

        // left face (normal: -1, 0, 0)
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 1, 0, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 0, 0, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(0, 0, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // right face (normal: 1, 0, 0)
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(0, 1, 0, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(0, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(1, 0, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(0, 0, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // top face (normal: 0, 1, 0)
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(1, 1, 0, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(0, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(0, 1, 0, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(1, 0, 0, 1), DirectX::XMFLOAT2(0, 1)},

        // bottom face (normal: 0, -1, 0)
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(0, 0, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(1, 0, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(0, 0, 0, 1), DirectX::XMFLOAT2(0, 1)},
    };

    UINT indices[] = {
        0, 1, 2, 0, 2, 3,       // front face
        4, 5, 6, 4, 6, 7,       // back face
        8, 9, 10, 8, 10, 11,    // left face
        12, 13, 14, 12, 14, 15, // right face
        16, 17, 18, 16, 18, 19, // top face
        20, 21, 22, 20, 22, 23  // bottom face
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;
    HRESULT hr = d3dDevice->CreateBuffer(&vbDesc, &vbData, vertexBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;
    hr = d3dDevice->CreateBuffer(&ibDesc, &ibData, indexBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(MatrixBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = d3dDevice->CreateBuffer(&cbDesc, nullptr, matrixConstantBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

bool Direct3DRenderer::InitializeLightingBuffer()
{
    LightBuffer lb;
    lb.ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    lb.diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    lb.lightDirection = DirectX::XMFLOAT3(0.577f, -0.577f, 0.577f);
    lb.padding = 0.0f;

    D3D11_BUFFER_DESC lbDesc = {};
    lbDesc.Usage = D3D11_USAGE_DEFAULT;
    lbDesc.ByteWidth = sizeof(LightBuffer);
    lbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lbDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA lbData = {};
    lbData.pSysMem = &lb;
    HRESULT hr = d3dDevice->CreateBuffer(&lbDesc, &lbData, lightConstantBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

void Direct3DRenderer::Update()
{
    UpdateTiming();
    UpdateCubeRotation();
    UpdateCameraMovement();
}

void Direct3DRenderer::UpdateTiming()
{
    auto now = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    fpsTimeAccumulator += deltaTime;
    fpsFrameCount++;
    if (fpsTimeAccumulator > 1.0f)
    {
        currentFPS = fpsFrameCount;
        fpsTimeAccumulator = 0.0f;
        fpsFrameCount = 0;
    }
}

void Direct3DRenderer::UpdateCubeRotation()
{
    const float rotationSpeed = 1.0f;
    cubeRotationAngle += rotationSpeed * deltaTime;
}

void Direct3DRenderer::UpdateCameraMovement()
{
    // Local using for DirectX improves readability in this function.
    using namespace DirectX;

    const float moveSpeed = 2.0f;
    float cosPitch = cosf(camera.pitch);
    float sinPitch = sinf(camera.pitch);
    float cosYaw = cosf(camera.yaw);
    float sinYaw = sinf(camera.yaw);

    XMVECTOR forward = XMVectorSet(cosPitch * sinYaw, sinPitch, cosPitch * cosYaw, 0.0f);
    forward = XMVector3Normalize(forward);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMVECTOR right = XMVector3Cross(up, forward);
    right = XMVector3Normalize(right);

    if (GetAsyncKeyState('W') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&camera.position);
        pos = XMVectorAdd(pos, XMVectorScale(forward, moveSpeed * deltaTime));
        XMStoreFloat3(&camera.position, pos);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&camera.position);
        pos = XMVectorSubtract(pos, XMVectorScale(forward, moveSpeed * deltaTime));
        XMStoreFloat3(&camera.position, pos);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&camera.position);
        pos = XMVectorSubtract(pos, XMVectorScale(right, moveSpeed * deltaTime));
        XMStoreFloat3(&camera.position, pos);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&camera.position);
        pos = XMVectorAdd(pos, XMVectorScale(right, moveSpeed * deltaTime));
        XMStoreFloat3(&camera.position, pos);
    }
}

void Direct3DRenderer::Render()
{
    d3dContext->RSSetState(isWireframeEnabled ? wireframeRasterizerState.Get() : solidRasterizerState.Get());
    ResetRenderStates();
    ClearBuffers();

    DirectX::XMMATRIX view, projection;
    ConfigureCameraMatrices(view, projection);

    d3dContext->PSSetShaderResources(0, 1, textureResourceView.GetAddressOf());
    d3dContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    d3dContext->PSSetConstantBuffers(1, 1, lightConstantBuffer.GetAddressOf());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    d3dContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    d3dContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    DirectX::XMMATRIX world1 = DirectX::XMMatrixRotationY(cubeRotationAngle) * DirectX::XMMatrixTranslation(-1.5f, 0.0f, 0.0f);
    DrawCube(world1, view, projection);

    DirectX::XMMATRIX world2 = DirectX::XMMatrixRotationX(cubeRotationAngle) * DirectX::XMMatrixTranslation(1.5f, 0.0f, 0.0f);
    DrawCube(world2, view, projection);

    RenderHUD();
    swapChain->Present(0, 0);
}

void Direct3DRenderer::ClearBuffers()
{
    float clearColor[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    d3dContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    d3dContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Direct3DRenderer::ConfigureCameraMatrices(DirectX::XMMATRIX &view, DirectX::XMMATRIX &projection)
{
    using namespace DirectX;

    XMVECTOR eye = XMLoadFloat3(&camera.position);
    float cosPitch = cosf(camera.pitch);
    float sinPitch = sinf(camera.pitch);
    float cosYaw = cosf(camera.yaw);
    float sinYaw = sinf(camera.yaw);

    XMVECTOR forward = XMVectorSet(cosPitch * sinYaw, sinPitch, cosPitch * cosYaw, 0.0f);
    forward = XMVector3Normalize(forward);
    XMVECTOR target = XMVectorAdd(eye, forward);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    view = XMMatrixLookAtLH(eye, target, up);

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, aspectRatio, 0.01f, 100.0f);
}

void Direct3DRenderer::DrawCube(const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection)
{
    MatrixBuffer mb;
    mb.world = DirectX::XMMatrixTranspose(world);
    mb.wvp = DirectX::XMMatrixTranspose(world * view * projection);
    d3dContext->UpdateSubresource(matrixConstantBuffer.Get(), 0, nullptr, &mb, 0, 0);
    d3dContext->VSSetConstantBuffers(0, 1, matrixConstantBuffer.GetAddressOf());
    d3dContext->DrawIndexed(36, 0, 0);
}

void Direct3DRenderer::RenderHUD()
{
    spriteBatch->Begin();
    wchar_t hudText[256];
    swprintf_s(hudText, L"FPS: %d\nCamera: (%.2f, %.2f, %.2f)\nWindow: %d x %d",
               currentFPS, camera.position.x, camera.position.y, camera.position.z,
               windowWidth, windowHeight);
    spriteFont->DrawString(spriteBatch.get(), hudText, DirectX::XMFLOAT2(10, 10), DirectX::Colors::White);
    spriteBatch->End();
}

void Direct3DRenderer::ResetRenderStates()
{
    d3dContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
    d3dContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    d3dContext->IASetInputLayout(inputLayout.Get());
    d3dContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    d3dContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    d3dContext->PSSetShaderResources(0, 1, textureResourceView.GetAddressOf());
    d3dContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    d3dContext->RSSetState(isWireframeEnabled ? wireframeRasterizerState.Get() : solidRasterizerState.Get());
}

bool Direct3DRenderer::OnResize(UINT newWidth, UINT newHeight)
{
    windowWidth = newWidth;
    windowHeight = newHeight;

    renderTargetView.Reset();
    depthStencilView.Reset();

    HRESULT hr = swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                              reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = newWidth;
    depthDesc.Height = newHeight;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    hr = d3dDevice->CreateTexture2D(&depthDesc, nullptr, depthBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateDepthStencilView(depthBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
    if (FAILED(hr))
        return false;

    ResetRenderStates();

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(newWidth);
    vp.Height = static_cast<FLOAT>(newHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dContext->RSSetViewports(1, &vp);
    return true;
}

LRESULT CALLBACK Direct3DRenderer::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    Direct3DRenderer *renderer = reinterpret_cast<Direct3DRenderer *>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
    switch (message)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        Direct3DRenderer *rendererPtr = reinterpret_cast<Direct3DRenderer *>(createStruct->lpCreateParams);
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(rendererPtr));
        return 0;
    }
    case WM_SIZE:
        if (renderer && renderer->swapChain)
            renderer->OnResize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (renderer && wParam == 'R')
            renderer->isWireframeEnabled = !renderer->isWireframeEnabled;
        break;
    case WM_LBUTTONDOWN:
        if (renderer)
        {
            renderer->camera.isMouseDown = true;
            renderer->camera.prevPosition.x = GET_X_LPARAM(lParam);
            renderer->camera.prevPosition.y = GET_Y_LPARAM(lParam);
        }
        break;
    case WM_LBUTTONUP:
        if (renderer)
            renderer->camera.isMouseDown = false;
        break;
    case WM_MOUSEMOVE:
        if (renderer && renderer->camera.isMouseDown)
        {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            int deltaX = mouseX - renderer->camera.prevPosition.x;
            int deltaY = mouseY - renderer->camera.prevPosition.y;
            float sensitivity = 0.002f;
            renderer->camera.yaw += deltaX * sensitivity;
            renderer->camera.pitch -= deltaY * sensitivity;
            if (renderer->camera.pitch > DirectX::XM_PIDIV2 - 0.1f)
                renderer->camera.pitch = DirectX::XM_PIDIV2 - 0.1f;
            if (renderer->camera.pitch < -DirectX::XM_PIDIV2 + 0.1f)
                renderer->camera.pitch = -DirectX::XM_PIDIV2 + 0.1f;
            renderer->camera.prevPosition.x = mouseX;
            renderer->camera.prevPosition.y = mouseY;
        }
        break;
    default:
        return DefWindowProc(windowHandle, message, wParam, lParam);
    }
    return 0;
}

void Direct3DRenderer::Run()
{
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
        }
    }
}

int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE, LPWSTR, int)
{
    Direct3DRenderer renderer(instanceHandle);
    if (!renderer.Initialize())
    {
        MessageBox(nullptr, L"Failed to initialize Direct3D Renderer.", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    renderer.Run();
    return 0;
}