#pragma once

#include "Registry.h"
#include "Managers/MeshManager.h"
#include "Managers/ShaderManager.h"
#include "ResourceManager.h"
#include <DirectXMath.h>
#include <string>
#include <memory>

class Scene
{
public:
    Scene(Registry &registry, const std::string &name = "Default Scene");
    ~Scene();

    void Initialize(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<ResourceManager> resourceManager);
    void Update(float deltaTime);

    EntityID CreateCube(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale = {1.0f, 1.0f, 1.0f});
    EntityID CreateCamera(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &lookDirection);
    EntityID CreateLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT4 color);

    std::string GetName() const { return name; }

private:
    Registry &registry;
    std::string name;

    std::shared_ptr<MeshManager> meshManager = nullptr;
    std::shared_ptr<ResourceManager> resourceManager = nullptr;

    MeshData cubeMeshData;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defaultDiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defaultNormalTexture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSamplerState;
};