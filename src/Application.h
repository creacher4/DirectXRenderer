#pragma once

#include "Registry.h"
#include "Systems/RenderSystem.h"
#include "Systems/InputSystem.h"
#include "Scene.h"
#include "Core/WindowManager.h"
#include "Core/Timer.h"
#include <windows.h>
#include <string>
#include <memory>

// documentation needed
class Application
{
public:
    Application(HINSTANCE hInstance);
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    bool InitializeSystems();
    void CreateScene();
    void OnResize(UINT width, UINT height);
    void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

    HINSTANCE hInstance;
    std::unique_ptr<WindowManager> windowManager;
    std::unique_ptr<Timer> timer;

    Registry registry;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<InputSystem> inputSystem;
    std::unique_ptr<Scene> currentScene;

    bool isRunning = true;
};