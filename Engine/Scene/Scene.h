#pragma once

#include "../Engine/ECS/Registry.h"
#include "../Engine/Resources/MeshManager.h"
#include "../Engine/Resources/ShaderManager.h"
#include "../Engine/Resources/ResourceManager.h"
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

    // creating primitive meshes
    EntityID CreateCube(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale = {1.0f, 1.0f, 1.0f});
    EntityID CreateSphere(float radius, int slices, int stacks,
                          const DirectX::XMFLOAT3 &position,
                          const DirectX::XMFLOAT3 &scale = {1.0f, 1.0f, 1.0f});
    EntityID CreateGroundPlane(float width, float depth, int xDivs, int zDivs,
                               const DirectX::XMFLOAT3 &position = {0.0f, 0.0f, 0.0f});

    // creating camera
    // i wonder if there are different camera types
    EntityID CreateCamera(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &lookDirection);

    // creating light types
    EntityID CreateLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT4 color);
    EntityID CreatePointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, float range = 10.0f);
    EntityID CreateSpotLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction,
                             DirectX::XMFLOAT4 color, float range = 10.0f);

    std::string GetName() const { return name; }

private:
    Registry &registry;
    std::string name;

    // ??? nullptrs??????????????????????????
    std::shared_ptr<MeshManager> meshManager = nullptr;
    std::shared_ptr<ResourceManager> resourceManager = nullptr;

    MeshData cubeMeshData;
    MeshData sphereMeshData;
    MeshData planeMeshData;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defaultDiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defaultNormalTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundDiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundNormalTexture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSamplerState;
};