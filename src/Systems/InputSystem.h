#pragma once

#include "System.h"
#include <windows.h>
#include <DirectXMath.h>

class InputSystem : public System
{
public:
    InputSystem(Registry &registry, HWND windowHandle);
    ~InputSystem() = default;

    bool Initialize() override;
    void Update(float deltaTime) override;

    void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

    bool IsKeyPressed(int keyCode) const;
    bool IsMouseButtonPressed(int button) const;
    DirectX::XMFLOAT2 GetMousePosition() const;
    DirectX::XMFLOAT2 GetMouseDelta() const;

private:
    HWND windowHandle;
    bool keys[256] = {false};
    bool mouseButtons[3] = {false};
    DirectX::XMFLOAT2 mousePosition = {0.0f, 0.0f};
    DirectX::XMFLOAT2 lastMousePosition = {0.0f, 0.0f};
    DirectX::XMFLOAT2 mouseDelta = {0.0f, 0.0f};
    bool firstFrame = true;
};