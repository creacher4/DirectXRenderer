#include "InputSystem.h"
#include "../Components/CameraComponent.h"
#include <algorithm>

InputSystem::InputSystem(Registry &registry, HWND windowHandle)
    : System(registry), windowHandle(windowHandle)
{
}

bool InputSystem::Initialize()
{
    return true;
}

void InputSystem::Update(float deltaTime)
{
    for (int i = 0; i < 256; i++)
    {
        keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

    POINT currentPos;
    if (GetCursorPos(&currentPos) && ScreenToClient(windowHandle, &currentPos))
    {
        DirectX::XMFLOAT2 newPosition = {
            static_cast<float>(currentPos.x),
            static_cast<float>(currentPos.y)};

        if (!firstFrame)
        {
            mouseDelta.x = newPosition.x - mousePosition.x;
            mouseDelta.y = newPosition.y - mousePosition.y;
        }
        else
        {
            mouseDelta = {0.0f, 0.0f};
            firstFrame = false;
        }

        mousePosition = newPosition;

        mouseButtons[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        mouseButtons[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        mouseButtons[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
    }

    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
    for (auto entity : cameraEntities)
    {
        if (auto *camera = registry.GetComponent<CameraComponent>(entity))
        {
            camera->moveForward = IsKeyPressed('W');
            camera->moveBackward = IsKeyPressed('S');
            camera->moveLeft = IsKeyPressed('A');
            camera->moveRight = IsKeyPressed('D');
            camera->moveUp = IsKeyPressed('E');
            camera->moveDown = IsKeyPressed('Q');

            if (IsMouseButtonPressed(1)) // right mouse button
            {
                const float sensitivity = 0.003f;

                DirectX::XMVECTOR lookDir = DirectX::XMLoadFloat3(&camera->lookDirection);
                DirectX::XMVECTOR rightAxis = DirectX::XMVector3Cross(
                    DirectX::XMVectorSet(0, 1, 0, 0), lookDir);
                rightAxis = DirectX::XMVector3Normalize(rightAxis);

                DirectX::XMVECTOR quatX = DirectX::XMQuaternionRotationAxis(
                    rightAxis, mouseDelta.y * sensitivity);
                DirectX::XMVECTOR quatY = DirectX::XMQuaternionRotationAxis(
                    DirectX::XMVectorSet(0, 1, 0, 0), mouseDelta.x * sensitivity);

                DirectX::XMVECTOR finalQuat = DirectX::XMQuaternionMultiply(quatX, quatY);
                lookDir = DirectX::XMVector3Rotate(lookDir, finalQuat);
                lookDir = DirectX::XMVector3Normalize(lookDir);

                DirectX::XMStoreFloat3(&camera->lookDirection, lookDir);
            }
        }
    }
}

// bro idek what the hell this is
// i feel like the 'separation of concerns' principle is being violated here
// but i've got to sit around and actually think about where im going wrong
void InputSystem::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        keys[wParam] = true;
        break;
    case WM_KEYUP:
        keys[wParam] = false;
        break;
    case WM_LBUTTONDOWN:
        mouseButtons[0] = true;
        break;
    case WM_LBUTTONUP:
        mouseButtons[0] = false;
        break;
    case WM_RBUTTONDOWN:
        mouseButtons[1] = true;
        break;
    case WM_RBUTTONUP:
        mouseButtons[1] = false;
        break;
    case WM_MBUTTONDOWN:
        mouseButtons[2] = true;
        break;
    case WM_MBUTTONUP:
        mouseButtons[2] = false;
        break;
    }
}

bool InputSystem::IsKeyPressed(int keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;
    return keys[keyCode];
}

bool InputSystem::IsMouseButtonPressed(int button) const
{
    if (button < 0 || button >= 3)
        return false;
    return mouseButtons[button];
}

DirectX::XMFLOAT2 InputSystem::GetMousePosition() const
{
    return mousePosition;
}

DirectX::XMFLOAT2 InputSystem::GetMouseDelta() const
{
    return mouseDelta;
}