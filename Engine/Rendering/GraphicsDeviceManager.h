#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <DirectXMath.h>

class ResourceManager;

// documentation coming soon i promise
class GraphicsDeviceManager
{
public:
    GraphicsDeviceManager(HWND windowHandle, UINT width, UINT height);
    ~GraphicsDeviceManager();

    bool Initialize();
    bool OnResize(UINT newWidth, UINT newHeight);

    ID3D11Device *GetDevice() const { return d3dDevice.Get(); }
    ID3D11DeviceContext *GetContext() const { return d3dContext.Get(); }

    IDXGISwapChain *GetSwapChain() const { return swapChain.Get(); }
    ID3D11RenderTargetView *GetRenderTargetView() const { return renderTargetView.Get(); }
    ID3D11DepthStencilView *GetDepthStencilView() const { return depthStencilView.Get(); }

    ID3D11DepthStencilState *GetDepthStencilState() const { return depthStencilState.Get(); }
    ID3D11RasterizerState *GetSolidRasterizerState() const { return solidRasterizerState.Get(); }
    ID3D11RasterizerState *GetWireframeRasterizerState() const { return wireframeRasterizerState.Get(); }

    void ClearBuffers(const float clearColor[4]);
    void Present();

    UINT GetWidth() const { return windowWidth; }
    UINT GetHeight() const { return windowHeight; }

private:
    bool CreateD3DDeviceAndSwapChain();
    bool InitializeRasterizerStates();
    bool InitializeRenderTargetAndDepthBuffer();
    bool ConfigureViewport();

    HWND windowHandle;
    UINT windowWidth;
    UINT windowHeight;

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterizerState;

    std::shared_ptr<ResourceManager> resourceManager;
};
