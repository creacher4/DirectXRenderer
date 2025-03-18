#pragma once

#include "Component.h"
#include <DirectXMath.h>

// needs fixing
class BaseLightComponent : public Component
{
public:
    // common light properties
    DirectX::XMFLOAT4 ambientColor = {0.2f, 0.2f, 0.2f, 1.0f};
    DirectX::XMFLOAT4 diffuseColor = {0.8f, 0.8f, 0.8f, 1.0f};
    float intensity = 1.0f;
    bool isEnabled = true;

    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    virtual LightType GetLightType() const = 0;
};

// sunlight
class DirectionalLightComponent : public BaseLightComponent
{
public:
    // directional light properties
    DirectX::XMFLOAT3 direction = {0.57735f, -0.57735f, 0.57735f}; // default direction

    LightType GetLightType() const override
    {
        return LightType::Directional;
    }
};

// lightbulb
class PointLightComponent : public BaseLightComponent
{
public:
    // point light properties
    DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f}; // default position
    float range = 10.0f;

    LightType GetLightType() const override
    {
        return LightType::Point;
    }
};

// flashlight
class SpotLightComponent : public BaseLightComponent
{
public:
    // spot light properties
    DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f};  // default position
    DirectX::XMFLOAT3 direction = {0.0f, 0.0f, 1.0f}; // default direction
    float range = 10.0f;
    float innerConeAngle = 0.0f;
    float outerConeAngle = DirectX::XM_PIDIV4;

    LightType GetLightType() const override
    {
        return LightType::Spot;
    }
};