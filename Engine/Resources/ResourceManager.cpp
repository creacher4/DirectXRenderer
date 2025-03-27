#include "ResourceManager.h"
#include <DirectXTK/WICTextureLoader.h>

ResourceManager::ResourceManager(ID3D11Device *device, ID3D11DeviceContext *context)
{
    this->device = device;
    this->context = context;
}

ResourceManager::~ResourceManager()
{
    Release();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ResourceManager::LoadTexture(const std::wstring &filename)
{
    auto it = textures.find(filename);
    if (it != textures.end())
    {
        return it->second;
    }

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        device.Get(), context.Get(), filename.c_str(), nullptr, textureView.GetAddressOf());

    if (FAILED(hr))
    {
        std::wstring msg = L"[ResourceManager] Failed to load texture: " + filename + L"\n";
        OutputDebugString(msg.c_str());
        return nullptr;
    }

    textures[filename] = textureView;
    return textureView;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> ResourceManager::CreateVertexBuffer(const void *data, UINT byteWidth)
{
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = byteWidth;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = data;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, buffer.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create vertex buffer\n");
        return nullptr;
    }

    return buffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> ResourceManager::CreateIndexBuffer(const void *data, UINT byteWidth)
{
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = byteWidth;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = data;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    HRESULT hr = device->CreateBuffer(&ibDesc, &ibData, buffer.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create index buffer\n");
        return nullptr;
    }

    return buffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> ResourceManager::CreateConstantBuffer(UINT byteWidth, const void *initialData)
{
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = byteWidth;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    HRESULT hr;

    if (initialData)
    {
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = initialData;
        hr = device->CreateBuffer(&cbDesc, &initData, buffer.GetAddressOf());
    }
    else
    {
        hr = device->CreateBuffer(&cbDesc, nullptr, buffer.GetAddressOf());
    }

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create constant buffer\n");
        return nullptr;
    }

    return buffer;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ResourceManager::CreateRenderTargetView(Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer)
{
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    HRESULT hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create render target view\n");
        return nullptr;
    }

    return renderTargetView;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> ResourceManager::CreateDepthStencilTexture(UINT width, UINT height)
{
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
    HRESULT hr = device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilTexture.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create depth stencil texture\n");
        return nullptr;
    }

    return depthStencilTexture;
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ResourceManager::CreateDepthStencilView(Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture)
{
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    HRESULT hr = device->CreateDepthStencilView(depthStencilTexture.Get(), nullptr, depthStencilView.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create depth stencil view\n");
        return nullptr;
    }

    return depthStencilView;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> ResourceManager::CreateSamplerState()
{
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    HRESULT hr = device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create sampler state\n");
        return nullptr;
    }

    return samplerState;
}

Microsoft::WRL::ComPtr<ID3D11RasterizerState> ResourceManager::CreateRasterizerState(bool wireframe)
{
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthClipEnable = TRUE;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create rasterizer state\n");
        return nullptr;
    }

    return rasterizerState;
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> ResourceManager::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugString(L"[ResourceManager] Failed to create depth stencil state\n");
        return nullptr;
    }

    return depthStencilState;
}

void ResourceManager::Release()
{
    textures.clear();
}