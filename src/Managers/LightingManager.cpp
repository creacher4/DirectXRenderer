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
    // this HAS to be suboptimal in some way
    // this will be sorted out later
    // atm, you can only switch between the different light types by changing the type in the editor
    // this is NOT the way i wanted to do it originally but will keep it like this until i create a separate gui system
    LightBuffer lightData = {};
    lightData.ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    lightData.diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    lightData.lightDirection = DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f);
    lightData.padding1 = 0.0f;
    lightData.lightPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    lightData.lightRange = 100.0f;
    lightData.spotInnerCone = cosf(DirectX::XMConvertToRadians(30.0f));
    lightData.spotOuterCone = cosf(DirectX::XMConvertToRadians(45.0f));
    lightData.lightType = 0;
    lightData.lightIntensity = 1.0f;

    auto resourceManager = std::make_shared<ResourceManager>(device.Get(), context.Get());
    lightConstantBuffer = resourceManager->CreateConstantBuffer(sizeof(LightBuffer), &lightData);

    return lightConstantBuffer != nullptr;
}

void LightingManager::Update()
{
    LightBuffer lightData = {};
    lightData.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
    lightData.lightType = 0;

    // now we can just go through each light type entity separately

    auto directionalLights = registry.GetEntitiesWith<DirectionalLightComponent>();
    if (!directionalLights.empty())
    {
        auto entity = *directionalLights.begin();
        auto *light = registry.GetComponent<DirectionalLightComponent>(entity);

        if (light)
        {
            lightData.ambientColor = light->ambientColor;
            lightData.diffuseColor = light->diffuseColor;
            lightData.lightIntensity = light->intensity;

            // everything after this is for that specific light type
            // obviously we could type 0 here but like ehhhh
            lightData.lightType = static_cast<int>(light->GetLightType());
            lightData.lightDirection = light->direction;
        }
    }

    // overrides directional light if there is a point light

    auto pointLights = registry.GetEntitiesWith<PointLightComponent>();
    if (!pointLights.empty())
    {
        auto entity = *pointLights.begin();
        auto *light = registry.GetComponent<PointLightComponent>(entity);

        if (light)
        {
            lightData.ambientColor = light->ambientColor;
            lightData.diffuseColor = light->diffuseColor;
            lightData.lightIntensity = light->intensity;

            // everything after this is for that specific light type
            // obviously we could type 1 here but like ehhhh
            lightData.lightType = static_cast<int>(light->GetLightType());
            lightData.lightPosition = light->position;
            lightData.lightRange = light->range;
        }
    }

    // overrides everything else since it's last

    auto spotLights = registry.GetEntitiesWith<SpotLightComponent>();
    if (!spotLights.empty())
    {
        auto entity = *spotLights.begin();
        auto *light = registry.GetComponent<SpotLightComponent>(entity);

        if (light)
        {
            lightData.ambientColor = light->ambientColor;
            lightData.diffuseColor = light->diffuseColor;
            lightData.lightIntensity = light->intensity;

            // everything after this is for that specific light type
            // obviously we could type 2 here but like ehhhh
            lightData.lightType = static_cast<int>(light->GetLightType());
            lightData.lightPosition = light->position;
            lightData.lightDirection = light->direction;
            lightData.lightRange = light->range;
            // convert to radians maybe?
            lightData.spotInnerCone = cosf(light->innerConeAngle);
            lightData.spotOuterCone = cosf(light->outerConeAngle);
        }
    }

    context->UpdateSubresource(lightConstantBuffer.Get(), 0, nullptr, &lightData, 0, 0);
}

void LightingManager::Apply(UINT slot)
{
    context->PSSetConstantBuffers(slot, 1, lightConstantBuffer.GetAddressOf());
}