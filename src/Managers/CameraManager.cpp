#include "CameraManager.h"

using namespace DirectX;

CameraManager::CameraManager(Registry &registry)
    : registry(registry)
{
}

void CameraManager::ConfigureCameraMatrices(DirectX::XMMATRIX &view, DirectX::XMMATRIX &projection, UINT windowWidth, UINT windowHeight)
{
    // supporting multiple cameras or a dedicated 'main' camera rather than always picking the first one was the original plan
    // will leave it for future me to solve
    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();

    if (!cameraEntities.empty())
    {
        auto entity = *cameraEntities.begin();
        auto *camera = registry.GetComponent<CameraComponent>(entity);
        auto *transform = registry.GetComponent<TransformComponent>(entity);

        if (camera && transform)
        {
            XMVECTOR eyePosition = XMLoadFloat3(&transform->position);
            XMVECTOR lookDir = XMLoadFloat3(&camera->lookDirection);
            XMVECTOR focusPoint = eyePosition + lookDir;
            XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);

            view = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
            projection = XMMatrixPerspectiveFovLH(camera->fov, camera->aspectRatio, camera->nearPlane, camera->farPlane);
            return;
        }
    }

    XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
    XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

    view = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
    projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.01f, 100.0f);
}

void CameraManager::UpdateCameraMovement(float deltaTime)
{
    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
    if (cameraEntities.empty())
        return;

    auto entity = *cameraEntities.begin();
    auto *camera = registry.GetComponent<CameraComponent>(entity);
    auto *transform = registry.GetComponent<TransformComponent>(entity);

    if (!camera || !transform)
        return;

    XMVECTOR lookDir = XMLoadFloat3(&camera->lookDirection);
    XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR rightVector = XMVector3Cross(upVector, lookDir);
    rightVector = XMVector3Normalize(rightVector);

    upVector = XMVector3Cross(lookDir, rightVector);

    float moveAmount = camera->moveSpeed * deltaTime;

    // might move movement logic to a separate function
    if (camera->moveForward)
    {
        XMVECTOR pos = XMLoadFloat3(&transform->position);
        pos = XMVectorAdd(pos, XMVectorScale(lookDir, moveAmount));
        XMStoreFloat3(&transform->position, pos);
    }
    if (camera->moveBackward)
    {
        XMVECTOR pos = XMLoadFloat3(&transform->position);
        pos = XMVectorSubtract(pos, XMVectorScale(lookDir, moveAmount));
        XMStoreFloat3(&transform->position, pos);
    }

    if (camera->moveLeft)
    {
        XMVECTOR pos = XMLoadFloat3(&transform->position);
        pos = XMVectorSubtract(pos, XMVectorScale(rightVector, moveAmount));
        XMStoreFloat3(&transform->position, pos);
    }
    if (camera->moveRight)
    {
        XMVECTOR pos = XMLoadFloat3(&transform->position);
        pos = XMVectorAdd(pos, XMVectorScale(rightVector, moveAmount));
        XMStoreFloat3(&transform->position, pos);
    }

    if (camera->moveUp)
    {
        XMVECTOR pos = XMLoadFloat3(&transform->position);
        pos = XMVectorAdd(pos, XMVectorScale(upVector, moveAmount));
        XMStoreFloat3(&transform->position, pos);
    }
    if (camera->moveDown)
    {
        XMVECTOR pos = XMLoadFloat3(&transform->position);
        pos = XMVectorSubtract(pos, XMVectorScale(upVector, moveAmount));
        XMStoreFloat3(&transform->position, pos);
    }

    lookDir = XMVector3Normalize(lookDir);
    XMStoreFloat3(&camera->lookDirection, lookDir);

    camera->target.x = transform->position.x + camera->lookDirection.x;
    camera->target.y = transform->position.y + camera->lookDirection.y;
    camera->target.z = transform->position.z + camera->lookDirection.z;
}

EntityID CameraManager::FindMainCameraEntity()
{
    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
    return cameraEntities.empty() ? INVALID_ENTITY : *cameraEntities.begin();
}

void CameraManager::UpdateAspectRatio(UINT width, UINT height)
{
    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
    for (auto entity : cameraEntities)
    {
        if (auto *camera = registry.GetComponent<CameraComponent>(entity))
        {
            camera->aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        }
    }
}

bool CameraManager::GetCameraPosition(DirectX::XMFLOAT3 &position)
{
    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
    if (cameraEntities.empty())
        return false;

    auto entity = *cameraEntities.begin();
    auto *transform = registry.GetComponent<TransformComponent>(entity);

    if (transform)
    {
        position = transform->position;
        return true;
    }

    return false;
}
