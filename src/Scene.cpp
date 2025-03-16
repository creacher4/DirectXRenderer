#include "Scene.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/MaterialComponent.h"

Scene::Scene(Registry &registry, const std::string &name)
    : registry(registry), name(name)
{
}

Scene::~Scene()
{
}

void Scene::Initialize(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<ResourceManager> resourceManager)
{
    this->meshManager = meshManager;
    this->resourceManager = resourceManager;

    if (meshManager && resourceManager)
    {
        cubeMeshData = meshManager->CreateCubeMesh();
        sphereMeshData = meshManager->CreateSphereMesh(1.0f, 20, 20);
        planeMeshData = meshManager->CreatePlaneMesh(10.0f, 10.0f, 10, 10);
        defaultDiffuseTexture = resourceManager->LoadTexture(L"assets/gray_rocks_diffuse.png");
        defaultNormalTexture = resourceManager->LoadTexture(L"assets/gray_rocks_normal.png");
        groundDiffuseTexture = resourceManager->LoadTexture(L"assets/ganges_river_pebbles_diffuse.png");
        groundNormalTexture = resourceManager->LoadTexture(L"assets/ganges_river_pebbles_normal.png");
        defaultSamplerState = resourceManager->CreateSamplerState();
    }
}

void Scene::Update(float deltaTime)
{
}

EntityID Scene::CreateCube(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale)
{
    EntityID entity = registry.CreateEntity();

    auto *transform = registry.AddComponent<TransformComponent>(entity);
    transform->position = position;
    transform->scale = scale;

    auto *mesh = registry.AddComponent<MeshComponent>(entity);
    if (cubeMeshData.vertexBuffer)
    {
        mesh->vertexBuffer = cubeMeshData.vertexBuffer;
        mesh->indexBuffer = cubeMeshData.indexBuffer;
        mesh->indexCount = cubeMeshData.indexCount;
        mesh->vertexStride = cubeMeshData.vertexStride;
    }

    auto *material = registry.AddComponent<MaterialComponent>(entity);
    if (defaultDiffuseTexture && defaultNormalTexture && defaultSamplerState)
    {
        material->diffuseTexture = defaultDiffuseTexture;
        material->normalTexture = defaultNormalTexture;
        material->samplerState = defaultSamplerState;
    }

    return entity;
}

EntityID Scene::CreateCamera(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &lookDirection)
{
    EntityID entity = registry.CreateEntity();

    auto *transform = registry.AddComponent<TransformComponent>(entity);
    transform->position = position;

    auto *camera = registry.AddComponent<CameraComponent>(entity);
    camera->lookDirection = lookDirection;

    return entity;
}

EntityID Scene::CreateLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT4 color)
{
    EntityID entity = registry.CreateEntity();

    auto *transform = registry.AddComponent<TransformComponent>(entity);

    auto *light = registry.AddComponent<LightComponent>(entity);
    light->direction = direction;
    light->diffuseColor = color;

    return entity;
}

EntityID Scene::CreateSphere(float radius, int slices, int stacks,
                             const DirectX::XMFLOAT3 &position,
                             const DirectX::XMFLOAT3 &scale)
{
    EntityID entity = registry.CreateEntity();

    // Add transform component
    auto *transform = registry.AddComponent<TransformComponent>(entity);
    transform->position = position;
    transform->scale = scale;

    // Add mesh component
    auto *mesh = registry.AddComponent<MeshComponent>(entity);
    if (sphereMeshData.vertexBuffer)
    {
        mesh->vertexBuffer = sphereMeshData.vertexBuffer;
        mesh->indexBuffer = sphereMeshData.indexBuffer;
        mesh->indexCount = sphereMeshData.indexCount;
        mesh->vertexStride = sphereMeshData.vertexStride;
    }

    // Add material component
    auto *material = registry.AddComponent<MaterialComponent>(entity);
    if (defaultDiffuseTexture && defaultNormalTexture && defaultSamplerState)
    {
        material->diffuseTexture = defaultDiffuseTexture;
        material->normalTexture = defaultNormalTexture;
        material->samplerState = defaultSamplerState;
    }

    return entity;
}

EntityID Scene::CreateGroundPlane(float width, float depth, int xDivs, int zDivs,
                                  const DirectX::XMFLOAT3 &position)
{
    EntityID entity = registry.CreateEntity();

    // Add transform component
    auto *transform = registry.AddComponent<TransformComponent>(entity);
    transform->position = position;

    // Add mesh component
    auto *mesh = registry.AddComponent<MeshComponent>(entity);
    if (planeMeshData.vertexBuffer)
    {
        mesh->vertexBuffer = planeMeshData.vertexBuffer;
        mesh->indexBuffer = planeMeshData.indexBuffer;
        mesh->indexCount = planeMeshData.indexCount;
        mesh->vertexStride = planeMeshData.vertexStride;
    }

    // Add material component
    auto *material = registry.AddComponent<MaterialComponent>(entity);
    if (groundDiffuseTexture && groundNormalTexture && defaultSamplerState)
    {
        material->diffuseTexture = groundDiffuseTexture;
        material->normalTexture = groundNormalTexture;
        material->samplerState = defaultSamplerState;
    }

    return entity;
}