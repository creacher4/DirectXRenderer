#pragma once

#include "System.h"
#include "../ResourceManager.h"
#include "../Managers/MeshManager.h"
#include "../Managers/ShaderManager.h"
#include "../Managers/LightingManager.h"
#include "../Managers/CameraManager.h"
#include "../Core/Timer.h"
#include "../Graphics/GraphicsDeviceManager.h"
#include "../Graphics/RenderPipelineManager.h"
#include "../Graphics/GUIManager.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// will add documentation
class RenderSystem : public System
{
public:
    RenderSystem(Registry &registry, HWND windowHandle, UINT windowWidth, UINT windowHeight, Timer &timer);
    virtual ~RenderSystem();

    bool Initialize() override;
    void Update(float deltaTime) override;
    void Render();
    bool OnResize(UINT newWidth, UINT newHeight);
    void ToggleWireframe()
    {
        isWireframeEnabled = !isWireframeEnabled;
        if (renderPipeline)
            renderPipeline->SetWireframeMode(isWireframeEnabled);
    }

    std::shared_ptr<MeshManager> GetMeshManager() const { return meshManager; }
    std::shared_ptr<ResourceManager> GetResourceManager() const { return resourceManager; }
    std::shared_ptr<ShaderManager> GetShaderManager() const { return shaderManager; }
    std::shared_ptr<GraphicsDeviceManager> GetGraphicsDevice() const { return graphicsDevice; }
    std::shared_ptr<RenderPipelineManager> GetRenderPipeline() const { return renderPipeline; }
    std::shared_ptr<GUIManager> GetGUIManager() const { return guiManager; }
    std::shared_ptr<CameraManager> GetCameraManager() const { return cameraManager; }

private:
    bool LoadDefaultTextures();

    void ConfigureCameraMatrices(DirectX::XMMATRIX &view, DirectX::XMMATRIX &projection);

    EntityID FindMainCameraEntity();
    void UpdateCameraMovement(float deltaTime);

    HWND windowHandle;
    UINT windowWidth;
    UINT windowHeight;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defaultDiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defaultNormalTexture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSamplerState;

    MeshData cubeMeshData;

    Timer &timer;

    bool isWireframeEnabled = false;
    float cubeRotationAngle = 0.0f;

    std::shared_ptr<GraphicsDeviceManager> graphicsDevice;
    std::shared_ptr<ResourceManager> resourceManager;
    std::shared_ptr<MeshManager> meshManager;
    std::shared_ptr<ShaderManager> shaderManager;
    std::shared_ptr<LightingManager> lightingManager;
    std::shared_ptr<RenderPipelineManager> renderPipeline;
    std::shared_ptr<GUIManager> guiManager;
    std::shared_ptr<CameraManager> cameraManager;
};