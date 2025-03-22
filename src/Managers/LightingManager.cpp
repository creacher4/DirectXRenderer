#include "LightingManager.h"
#include "../Components/TransformComponent.h"
#include <DirectXMath.h>
#include "../ResourceManager.h"

LightingManager::LightingManager(Microsoft::WRL::ComPtr<ID3D11Device> device,
                                 Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
                                 Registry &registry)
    : device(device), context(context), registry(registry)
{
}

LightingManager::~LightingManager()
{
}

bool LightingManager::Initialize()
{
    LightBuffer lightData = {};

    // set some default values for all lights
    // not sure if i want them all rn but it's okay
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        lightData.lights[i].ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
        lightData.lights[i].diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
        lightData.lights[i].lightDirection = DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f);
        lightData.lights[i].padding1 = 0.0f;
        lightData.lights[i].lightPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        lightData.lights[i].lightRange = 100.0f;
        lightData.lights[i].spotInnerCone = cosf(DirectX::XMConvertToRadians(30.0f));
        lightData.lights[i].spotOuterCone = cosf(DirectX::XMConvertToRadians(45.0f));
        lightData.lights[i].lightType = 0;
        lightData.lights[i].lightIntensity = 1.0f;
    }

    lightData.activeLightCount = 0;

    auto resourceManager = std::make_shared<ResourceManager>(device.Get(), context.Get());
    lightConstantBuffer = resourceManager->CreateConstantBuffer(sizeof(LightBuffer), &lightData);

    return lightConstantBuffer != nullptr;
}

void LightingManager::Update()
{
    LightBuffer lightData = {};
    int lightIndex = 0;

    // process each light type separately

    auto directionalLights = registry.GetEntitiesWith<DirectionalLightComponent>();
    for (auto entity : directionalLights)
    {
        if (lightIndex >= MAX_LIGHTS)
            break;

        auto *light = registry.GetComponent<DirectionalLightComponent>(entity);
        if (light && light->isEnabled)
        {
            lightData.lights[lightIndex].ambientColor = light->ambientColor;
            lightData.lights[lightIndex].diffuseColor = light->diffuseColor;
            lightData.lights[lightIndex].lightIntensity = light->intensity;
            lightData.lights[lightIndex].lightType = static_cast<int>(light->GetLightType());
            lightData.lights[lightIndex].lightDirection = light->direction;
            lightIndex++;
        }
    }

    auto pointLights = registry.GetEntitiesWith<PointLightComponent>();
    for (auto entity : pointLights)
    {
        if (lightIndex >= MAX_LIGHTS)
            break;

        auto *light = registry.GetComponent<PointLightComponent>(entity);
        if (light && light->isEnabled)
        {
            lightData.lights[lightIndex].ambientColor = light->ambientColor;
            lightData.lights[lightIndex].diffuseColor = light->diffuseColor;
            lightData.lights[lightIndex].lightIntensity = light->intensity;
            lightData.lights[lightIndex].lightType = static_cast<int>(light->GetLightType());
            lightData.lights[lightIndex].lightPosition = light->position;
            lightData.lights[lightIndex].lightRange = light->range;
            lightIndex++;
        }
    }

    auto spotLights = registry.GetEntitiesWith<SpotLightComponent>();
    for (auto entity : spotLights)
    {
        if (lightIndex >= MAX_LIGHTS)
            break;

        auto *light = registry.GetComponent<SpotLightComponent>(entity);
        if (light && light->isEnabled)
        {
            lightData.lights[lightIndex].ambientColor = light->ambientColor;
            lightData.lights[lightIndex].diffuseColor = light->diffuseColor;
            lightData.lights[lightIndex].lightIntensity = light->intensity;
            lightData.lights[lightIndex].lightType = static_cast<int>(light->GetLightType());
            lightData.lights[lightIndex].lightPosition = light->position;
            lightData.lights[lightIndex].lightDirection = light->direction;
            lightData.lights[lightIndex].lightRange = light->range;
            lightData.lights[lightIndex].spotInnerCone = cosf(light->innerConeAngle);
            lightData.lights[lightIndex].spotOuterCone = cosf(light->outerConeAngle);
            lightIndex++;
        }
    }

    lightData.activeLightCount = lightIndex;
    context->UpdateSubresource(lightConstantBuffer.Get(), 0, nullptr, &lightData, 0, 0);
}

void LightingManager::Apply(UINT slot)
{
    context->PSSetConstantBuffers(slot, 1, lightConstantBuffer.GetAddressOf());
}