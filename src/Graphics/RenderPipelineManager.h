#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include "../ResourceManager.h"
#include "GraphicsDeviceManager.h"
#include "../Buffers.h"

// documentation coming soon i promise
class RenderPipelineManager
{
public:
    RenderPipelineManager(std::shared_ptr<GraphicsDeviceManager> graphicsDevice,
                          std::shared_ptr<ResourceManager> resourceManager);
    ~RenderPipelineManager() = default;

    bool Initialize();

    void SetWireframeMode(bool enabled);
    void ResetRenderStates();
    void ClearBuffers(const float clearColor[4]);

    bool LoadDefaultShaders();
    bool CreateDefaultInputLayout(const void *shaderBytecode, size_t bytecodeLength);

    void DrawIndexed(UINT indexCount, UINT startIndex = 0, UINT baseVertex = 0);

    void SetVertexBuffer(ID3D11Buffer *vertexBuffer, UINT stride, UINT offset = 0);
    void SetIndexBuffer(ID3D11Buffer *indexBuffer, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT, UINT offset = 0);
    void SetConstantBuffer(ID3D11Buffer *constantBuffer, UINT slot, bool vertexShader = true, bool pixelShader = false);
    void SetTexture(ID3D11ShaderResourceView *texture, UINT slot);
    void SetSampler(ID3D11SamplerState *samplerState, UINT slot);

    void UpdateMatrixBuffer(const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);
    void UpdateCameraBuffer(const DirectX::XMFLOAT3 &cameraPosition);

    void Present();

    Microsoft::WRL::ComPtr<ID3D11InputLayout> GetDefaultInputLayout() const { return defaultInputLayout; }
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetDefaultVertexShader() const { return defaultVertexShader; }
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetDefaultPixelShader() const { return defaultPixelShader; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetMatrixBuffer() const { return matrixConstantBuffer; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetCameraBuffer() const { return cameraConstantBuffer; }

private:
    std::shared_ptr<GraphicsDeviceManager> graphicsDevice;
    std::shared_ptr<ResourceManager> resourceManager;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> defaultVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> defaultPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> defaultInputLayout;

    Microsoft::WRL::ComPtr<ID3D11Buffer> matrixConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> cameraConstantBuffer;

    bool isWireframeEnabled = false;
};
