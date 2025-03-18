#include "CameraComponent.h"

DirectX::XMMATRIX CameraComponent::GetViewMatrix() const
{
    using namespace DirectX;

    XMVECTOR eyePosition = XMLoadFloat3(&position);
    XMVECTOR lookDir = XMLoadFloat3(&lookDirection);
    XMVECTOR focusPoint = eyePosition + lookDir;
    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);

    return XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
}

DirectX::XMMATRIX CameraComponent::GetProjectionMatrix() const
{
    return DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}