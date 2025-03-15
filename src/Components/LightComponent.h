#pragma once

#include "Component.h"
#include <DirectXMath.h>

// needs fixing
class LightComponent : public Component
{
public:
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    LightType type = LightType::Directional;
    DirectX::XMFLOAT4 ambientColor = {0.2f, 0.2f, 0.2f, 1.0f};
    DirectX::XMFLOAT4 diffuseColor = {0.8f, 0.8f, 0.8f, 1.0f};
    DirectX::XMFLOAT3 direction = {0.0f, -1.0f, 1.0f};
    float range = 100.0f;
    float intensity = 1.0f;
};