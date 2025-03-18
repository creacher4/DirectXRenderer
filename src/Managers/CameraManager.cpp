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
        // camera component replaced with it's own unique transform component

        if (camera)
        {
            view = camera->GetViewMatrix();
            projection = camera->GetProjectionMatrix();
            return;
        }
    }

    // default just in case
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

    if (!camera)
        return;

    XMVECTOR lookDir = XMLoadFloat3(&camera->lookDirection);
    XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR rightVector = XMVector3Cross(upVector, lookDir);
    rightVector = XMVector3Normalize(rightVector);

    upVector = XMVector3Cross(lookDir, rightVector);

    float moveAmount = camera->moveSpeed * deltaTime;

    // might move movement logic to a separate function
    // we can use the camera to update the position rather than the mesh transform
    if (camera->moveForward)
    {
        XMVECTOR pos = XMLoadFloat3(&camera->position);
        pos = XMVectorAdd(pos, XMVectorScale(lookDir, moveAmount));
        XMStoreFloat3(&camera->position, pos);
    }
    if (camera->moveBackward)
    {
        XMVECTOR pos = XMLoadFloat3(&camera->position);
        pos = XMVectorSubtract(pos, XMVectorScale(lookDir, moveAmount));
        XMStoreFloat3(&camera->position, pos);
    }

    if (camera->moveLeft)
    {
        XMVECTOR pos = XMLoadFloat3(&camera->position);
        pos = XMVectorSubtract(pos, XMVectorScale(rightVector, moveAmount));
        XMStoreFloat3(&camera->position, pos);
    }
    if (camera->moveRight)
    {
        XMVECTOR pos = XMLoadFloat3(&camera->position);
        pos = XMVectorAdd(pos, XMVectorScale(rightVector, moveAmount));
        XMStoreFloat3(&camera->position, pos);
    }

    if (camera->moveUp)
    {
        XMVECTOR pos = XMLoadFloat3(&camera->position);
        pos = XMVectorAdd(pos, XMVectorScale(upVector, moveAmount));
        XMStoreFloat3(&camera->position, pos);
    }
    if (camera->moveDown)
    {
        XMVECTOR pos = XMLoadFloat3(&camera->position);
        pos = XMVectorSubtract(pos, XMVectorScale(upVector, moveAmount));
        XMStoreFloat3(&camera->position, pos);
    }

    lookDir = XMVector3Normalize(lookDir);
    XMStoreFloat3(&camera->lookDirection, lookDir);

    // again, updating target using the camera position transform rather than the mesh transform
    camera->target.x = camera->position.x + camera->lookDirection.x;
    camera->target.y = camera->position.y + camera->lookDirection.y;
    camera->target.z = camera->position.z + camera->lookDirection.z;
}

// not sure if im even using this function??
// maybe in a separate class or something
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