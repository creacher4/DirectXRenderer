#pragma once

#include "../../Engine/ECS/Registry.h"
#include "../../Engine/ECS/Components/TransformComponent.h"
#include "../../Engine/ECS/Components/CameraComponent.h"
#include <DirectXMath.h>
#include <Windows.h>

class CameraManager
{
public:
    CameraManager(Registry &registry);
    ~CameraManager() = default;

    void ConfigureCameraMatrices(DirectX::XMMATRIX &view, DirectX::XMMATRIX &projection, UINT windowWidth, UINT windowHeight);
    void UpdateCameraMovement(float deltaTime);

    EntityID FindMainCameraEntity();

    void UpdateAspectRatio(UINT width, UINT height);
    bool GetCameraPosition(DirectX::XMFLOAT3 &position);

private:
    Registry &registry;
};