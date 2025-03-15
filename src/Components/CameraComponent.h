#pragma once

#include "Component.h"
#include <DirectXMath.h>

class CameraComponent : public Component
{
public:
    DirectX::XMFLOAT3 lookDirection = {0.0f, 0.0f, 1.0f};
    DirectX::XMFLOAT3 target = {0.0f, 0.0f, 1.0f};

    float fov = DirectX::XM_PIDIV4;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float moveSpeed = 5.0f;

    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool moveUp = false;
    bool moveDown = false;
};