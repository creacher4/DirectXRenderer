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

    auto lightEntities = registry.GetEntitiesWith<LightComponent>();

    if (!lightEntities.empty())
    {
        auto entity = *lightEntities.begin();
        auto *light = registry.GetComponent<LightComponent>(entity);
        auto *transform = registry.GetComponent<TransformComponent>(entity);

        if (light && transform)
        {
            lightData.ambientColor = light->ambientColor;
            lightData.diffuseColor = light->diffuseColor;
            lightData.lightIntensity = light->intensity;

            lightData.lightType = static_cast<int>(light->type);

            switch (light->type)
            {
            case LightComponent::LightType::Directional:
                lightData.lightDirection = light->direction;
                break;

            case LightComponent::LightType::Point:
                lightData.lightPosition = transform->position;
                lightData.lightRange = light->range;
                break;

            case LightComponent::LightType::Spot:
                lightData.lightPosition = transform->position;
                lightData.lightDirection = light->direction;
                lightData.lightRange = light->range;
                lightData.spotInnerCone = cosf(DirectX::XMConvertToRadians(30.0f));
                lightData.spotOuterCone = cosf(DirectX::XMConvertToRadians(45.0f));
                break;
            }
        }
    }

    context->UpdateSubresource(lightConstantBuffer.Get(), 0, nullptr, &lightData, 0, 0);
}

void LightingManager::Apply(UINT slot)
{
    context->PSSetConstantBuffers(slot, 1, lightConstantBuffer.GetAddressOf());
}