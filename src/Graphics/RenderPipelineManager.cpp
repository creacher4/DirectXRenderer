#include "RenderPipelineManager.h"
#include "../Managers/ShaderManager.h"
#include <d3dcompiler.h>

RenderPipelineManager::RenderPipelineManager(std::shared_ptr<GraphicsDeviceManager> graphicsDevice,
                                             std::shared_ptr<ResourceManager> resourceManager)
    : graphicsDevice(graphicsDevice), resourceManager(resourceManager)
{
}

bool RenderPipelineManager::Initialize()
{
    matrixConstantBuffer = resourceManager->CreateConstantBuffer(sizeof(MatrixBuffer));
    if (!matrixConstantBuffer)
        return false;

    cameraConstantBuffer = resourceManager->CreateConstantBuffer(sizeof(CameraBuffer));
    if (!cameraConstantBuffer)
        return false;

    if (!LoadDefaultShaders())
        return false;

    return true;
}

bool RenderPipelineManager::LoadDefaultShaders()
{
    auto shaderManager = std::make_shared<ShaderManager>(resourceManager);

    Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
    if (!shaderManager->CompileAndCreateVertexShader(L"shaders/vertexShader.hlsl", "main", defaultVertexShader, vertexShaderBlob))
        return false;

    if (!shaderManager->CompileAndCreatePixelShader(L"shaders/pixelShader.hlsl", "main", defaultPixelShader))
        return false;

    return CreateDefaultInputLayout(vertexShaderBlob.Get()->GetBufferPointer(), vertexShaderBlob.Get()->GetBufferSize());
}

bool RenderPipelineManager::CreateDefaultInputLayout(const void *shaderBytecode, size_t bytecodeLength)
{
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    HRESULT hr = graphicsDevice->GetDevice()->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        shaderBytecode,
        bytecodeLength,
        defaultInputLayout.GetAddressOf());

    return SUCCEEDED(hr);
}

void RenderPipelineManager::SetWireframeMode(bool enabled)
{
    isWireframeEnabled = enabled;
}

void RenderPipelineManager::ResetRenderStates()
{
    auto d3dContext = graphicsDevice->GetContext();

    d3dContext->OMSetDepthStencilState(graphicsDevice->GetDepthStencilState(), 0);

    if (isWireframeEnabled)
        d3dContext->RSSetState(graphicsDevice->GetWireframeRasterizerState());
    else
        d3dContext->RSSetState(graphicsDevice->GetSolidRasterizerState());

    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dContext->IASetInputLayout(defaultInputLayout.Get());
    d3dContext->VSSetShader(defaultVertexShader.Get(), nullptr, 0);
    d3dContext->PSSetShader(defaultPixelShader.Get(), nullptr, 0);

    d3dContext->VSSetConstantBuffers(0, 1, matrixConstantBuffer.GetAddressOf());
}

void RenderPipelineManager::ClearBuffers(const float clearColor[4])
{
    graphicsDevice->ClearBuffers(clearColor);
}

void RenderPipelineManager::DrawIndexed(UINT indexCount, UINT startIndex, UINT baseVertex)
{
    graphicsDevice->GetContext()->DrawIndexed(indexCount, startIndex, baseVertex);
}

void RenderPipelineManager::SetVertexBuffer(ID3D11Buffer *vertexBuffer, UINT stride, UINT offset)
{
    graphicsDevice->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
}

void RenderPipelineManager::SetIndexBuffer(ID3D11Buffer *indexBuffer, DXGI_FORMAT format, UINT offset)
{
    graphicsDevice->GetContext()->IASetIndexBuffer(indexBuffer, format, offset);
}

void RenderPipelineManager::SetConstantBuffer(ID3D11Buffer *constantBuffer, UINT slot, bool vertexShader, bool pixelShader)
{
    if (vertexShader)
        graphicsDevice->GetContext()->VSSetConstantBuffers(slot, 1, &constantBuffer);

    if (pixelShader)
        graphicsDevice->GetContext()->PSSetConstantBuffers(slot, 1, &constantBuffer);
}

void RenderPipelineManager::SetTexture(ID3D11ShaderResourceView *texture, UINT slot)
{
    graphicsDevice->GetContext()->PSSetShaderResources(slot, 1, &texture);
}

void RenderPipelineManager::SetSampler(ID3D11SamplerState *samplerState, UINT slot)
{
    graphicsDevice->GetContext()->PSSetSamplers(slot, 1, &samplerState);
}

void RenderPipelineManager::UpdateMatrixBuffer(const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection)
{
    MatrixBuffer mb;
    mb.world = DirectX::XMMatrixTranspose(world);
    mb.wvp = DirectX::XMMatrixTranspose(world * view * projection);

    graphicsDevice->GetContext()->UpdateSubresource(matrixConstantBuffer.Get(), 0, nullptr, &mb, 0, 0);
}

void RenderPipelineManager::UpdateCameraBuffer(const DirectX::XMFLOAT3 &cameraPosition)
{
    CameraBuffer cb;
    cb.cameraPosition = cameraPosition;
    cb.padding = 0.0f;

    graphicsDevice->GetContext()->UpdateSubresource(cameraConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    graphicsDevice->GetContext()->PSSetConstantBuffers(2, 1, cameraConstantBuffer.GetAddressOf());
}

void RenderPipelineManager::Present()
{
    graphicsDevice->Present();
}
