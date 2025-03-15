#pragma once

#include "../Registry.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <string>
#include <memory>

class Timer;

// need a better gui tbh. might convert it from a manager to a system
// and maybe implement a gizmo system for moving entities around
class GUIManager
{
public:
    GUIManager(Registry &registry, HWND windowHandle, Timer &timer);
    ~GUIManager();

    bool Initialize(ID3D11Device *device, ID3D11DeviceContext *context);
    void NewFrame();
    void Render();
    void Shutdown();

    void ShowStatsWindow();
    void ShowMainMenuBar();
    void ShowEntityInspector();
    void ShowDemoWindow(bool *open);

    bool GetWireframeEnabled() const { return isWireframeEnabled; }

private:
    EntityID FindMainCameraEntity() const;

    Registry &registry;
    HWND windowHandle;
    Timer &timer;

    bool isWireframeEnabled = false;
    bool showDemoWindow = false;
};

// forward declare message handler from imgui_impl_win32.cpp
// not part of the class, so doesn't need to be at the top of the file
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
