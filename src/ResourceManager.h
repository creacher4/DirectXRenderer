#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <DirectXMath.h>

// needs documentation
class ResourceManager
{
public:
    ResourceManager(ID3D11Device *device, ID3D11DeviceContext *context);
    ~ResourceManager();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTexture(const std::wstring &filename);

    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(const void *data, UINT byteWidth);
    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(const void *data, UINT byteWidth);
    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(UINT byteWidth, const void *initialData = nullptr);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> CreateRenderTargetView(Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer);
    Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateDepthStencilTexture(UINT width, UINT height);
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> CreateDepthStencilView(Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture);

    Microsoft::WRL::ComPtr<ID3D11SamplerState> CreateSamplerState();
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> CreateRasterizerState(bool wireframe = false);
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CreateDepthStencilState();

    void Release();

    ID3D11Device *GetDevice() const { return device.Get(); }
    ID3D11DeviceContext *GetContext() const { return context.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures; // Stores loaded textures
};
