#pragma once

#include "../Registry.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
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