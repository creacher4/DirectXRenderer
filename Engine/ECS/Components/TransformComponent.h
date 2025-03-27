#pragma once

#include "../Component.h"
#include <DirectXMath.h>

// essentially the mesh transform component now
// i corrected the component implementations since they all use transforms differently
// especially the lights

class TransformComponent : public Component
{
public:
    DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 rotation = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};

    DirectX::XMMATRIX GetWorldMatrix() const;
};