
#include "TransformComponent.h"

DirectX::XMMATRIX TransformComponent::GetWorldMatrix() const
{
    using namespace DirectX;

    XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

    return scaleMatrix * rotationMatrix * translationMatrix;
}