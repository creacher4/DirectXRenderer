#include "GraphicsDeviceManager.h"
#include "../ResourceManager.h"

GraphicsDeviceManager::GraphicsDeviceManager(HWND windowHandle, UINT width, UINT height)
    : windowHandle(windowHandle), windowWidth(width), windowHeight(height)
{
}

GraphicsDeviceManager::~GraphicsDeviceManager()
{
}

bool GraphicsDeviceManager::Initialize()
{
    if (!CreateD3DDeviceAndSwapChain())
        return false;

    resourceManager = std::make_shared<ResourceManager>(d3dDevice.Get(), d3dContext.Get());

    if (!InitializeRasterizerStates())
        return false;
    if (!InitializeRenderTargetAndDepthBuffer())
        return false;
    if (!ConfigureViewport())
        return false;

    return true;
}

bool GraphicsDeviceManager::CreateD3DDeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = windowWidth;
    swapChainDesc.BufferDesc.Height = windowHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
        &swapChainDesc, swapChain.GetAddressOf(), d3dDevice.GetAddressOf(),
        nullptr, d3dContext.GetAddressOf());

    return SUCCEEDED(hr);
}

bool GraphicsDeviceManager::InitializeRasterizerStates()
{
    if (!resourceManager)
        return false;

    solidRasterizerState = resourceManager->CreateRasterizerState(false);
    wireframeRasterizerState = resourceManager->CreateRasterizerState(true);

    return solidRasterizerState && wireframeRasterizerState;
}

bool GraphicsDeviceManager::InitializeRenderTargetAndDepthBuffer()
{
    if (!resourceManager)
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    renderTargetView = resourceManager->CreateRenderTargetView(backBuffer);
    if (!renderTargetView)
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer = resourceManager->CreateDepthStencilTexture(windowWidth, windowHeight);
    if (!depthStencilBuffer)
        return false;

    depthStencilView = resourceManager->CreateDepthStencilView(depthStencilBuffer);
    if (!depthStencilView)
        return false;

    d3dContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    depthStencilState = resourceManager->CreateDepthStencilState();
    return depthStencilState != nullptr;
}

bool GraphicsDeviceManager::ConfigureViewport()
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(windowWidth);
    viewport.Height = static_cast<float>(windowHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    d3dContext->RSSetViewports(1, &viewport);

    return true;
}

void GraphicsDeviceManager::ClearBuffers(const float clearColor[4])
{
    d3dContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    d3dContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsDeviceManager::Present()
{
    swapChain->Present(0, 0);
}

bool GraphicsDeviceManager::OnResize(UINT newWidth, UINT newHeight)
{
    if (newWidth == 0 || newHeight == 0)
        return false;

    renderTargetView.Reset();
    depthStencilView.Reset();

    HRESULT hr = swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    renderTargetView = resourceManager->CreateRenderTargetView(backBuffer);
    if (!renderTargetView)
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer = resourceManager->CreateDepthStencilTexture(newWidth, newHeight);
    if (!depthBuffer)
        return false;

    depthStencilView = resourceManager->CreateDepthStencilView(depthBuffer);
    if (!depthStencilView)
        return false;

    d3dContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(newWidth);
    viewport.Height = static_cast<float>(newHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    d3dContext->RSSetViewports(1, &viewport);

    windowWidth = newWidth;
    windowHeight = newHeight;

    return true;
}