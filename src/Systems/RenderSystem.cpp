#include "RenderSystem.h"
#include "../Component.h"
#include "../Vertex.h"
#include "../Buffers.h"
#include "../Components/MeshComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/LightComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/MaterialComponent.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTK/WICTextureLoader.h>

using namespace DirectX;

RenderSystem::RenderSystem(Registry &registry, HWND windowHandle, UINT windowWidth, UINT windowHeight, Timer &timer)
    : System(registry), windowHandle(windowHandle), windowWidth(windowWidth), windowHeight(windowHeight), timer(timer)
{
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::Initialize()
{
    graphicsDevice = std::make_shared<GraphicsDeviceManager>(windowHandle, windowWidth, windowHeight);
    if (!graphicsDevice->Initialize())
        return false;

    resourceManager = std::make_shared<ResourceManager>(graphicsDevice->GetDevice(), graphicsDevice->GetContext());
    meshManager = std::make_shared<MeshManager>(resourceManager);
    shaderManager = std::make_shared<ShaderManager>(resourceManager);
    cameraManager = std::make_shared<CameraManager>(registry);
    lightingManager = std::make_shared<LightingManager>(
        Microsoft::WRL::ComPtr<ID3D11Device>(graphicsDevice->GetDevice()),
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>(graphicsDevice->GetContext()),
        registry);

    guiManager = std::make_shared<GUIManager>(registry, windowHandle, timer);
    if (!guiManager->Initialize(graphicsDevice->GetDevice(), graphicsDevice->GetContext()))
        return false;

    renderPipeline = std::make_shared<RenderPipelineManager>(graphicsDevice, resourceManager);
    if (!renderPipeline->Initialize())
        return false;

    cubeMeshData = meshManager->CreateCubeMesh();
    if (!cubeMeshData.vertexBuffer || !cubeMeshData.indexBuffer)
        return false;

    if (!LoadDefaultTextures())
        return false;

    if (!lightingManager->Initialize())
        return false;

    return true;
}

// this can be moved to a file manager class or something
bool RenderSystem::LoadDefaultTextures()
{
    defaultDiffuseTexture = resourceManager->LoadTexture(L"assets/gray_rocks_diffuse.png");
    if (!defaultDiffuseTexture)
        return false;

    defaultNormalTexture = resourceManager->LoadTexture(L"assets/gray_rocks_normal.png");
    if (!defaultNormalTexture)
        return false;

    defaultSamplerState = resourceManager->CreateSamplerState();
    if (!defaultSamplerState)
        return false;

    return true;
}

void RenderSystem::Update(float deltaTime)
{
    cameraManager->UpdateCameraMovement(deltaTime);

    if (isWireframeEnabled != guiManager->GetWireframeEnabled())
    {
        isWireframeEnabled = guiManager->GetWireframeEnabled();
        renderPipeline->SetWireframeMode(isWireframeEnabled);
    }
}

void RenderSystem::Render()
{
    float clearColor[] = {0.1f, 0.1f, 0.2f, 1.0f};
    renderPipeline->ClearBuffers(clearColor);
    renderPipeline->ResetRenderStates();

    XMMATRIX view, projection;
    cameraManager->ConfigureCameraMatrices(view, projection, windowWidth, windowHeight);

    DirectX::XMFLOAT3 cameraPosition;
    if (cameraManager->GetCameraPosition(cameraPosition))
    {
        renderPipeline->UpdateCameraBuffer(cameraPosition);
    }

    lightingManager->Update();
    lightingManager->Apply(1);

    renderPipeline->SetTexture(defaultDiffuseTexture.Get(), 0);
    renderPipeline->SetTexture(defaultNormalTexture.Get(), 1);
    renderPipeline->SetSampler(defaultSamplerState.Get(), 0);

    auto meshEntities = registry.GetEntitiesWith<MeshComponent>();
    for (auto entity : meshEntities)
    {
        auto *transform = registry.GetComponent<TransformComponent>(entity);
        auto *mesh = registry.GetComponent<MeshComponent>(entity);
        auto *material = registry.GetComponent<MaterialComponent>(entity);

        if (transform && mesh)
        {
            XMMATRIX world = transform->GetWorldMatrix();

            renderPipeline->UpdateMatrixBuffer(world, view, projection);

            renderPipeline->SetVertexBuffer(mesh->vertexBuffer.Get(), mesh->vertexStride);
            renderPipeline->SetIndexBuffer(mesh->indexBuffer.Get());

            if (material)
            {
                if (material->diffuseTexture)
                {
                    renderPipeline->SetTexture(material->diffuseTexture.Get(), 0);
                }

                if (material->normalTexture)
                {
                    renderPipeline->SetTexture(material->normalTexture.Get(), 1);
                }

                if (material->samplerState)
                {
                    renderPipeline->SetSampler(material->samplerState.Get(), 0);
                }
            }

            renderPipeline->DrawIndexed(mesh->indexCount);
        }
    }

    // can probably the gui rendering less wordy
    guiManager->NewFrame();
    guiManager->ShowStatsWindow();
    guiManager->ShowEntityInspector();

    // this can't be correct bro
    // need to test this
    static bool showDemoWindow = false;
    guiManager->ShowDemoWindow(&showDemoWindow);

    guiManager->Render();

    renderPipeline->Present();
}

bool RenderSystem::OnResize(UINT newWidth, UINT newHeight)
{
    if (!graphicsDevice->OnResize(newWidth, newHeight))
        return false;

    windowWidth = newWidth;
    windowHeight = newHeight;

    cameraManager->UpdateAspectRatio(newWidth, newHeight);

    return true;
}