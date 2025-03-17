#include "Application.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include <imgui.h>
#include <imgui_impl_win32.h>

Application::Application(HINSTANCE hInstance) : hInstance(hInstance)
{
    windowManager = std::make_unique<WindowManager>();
    timer = std::make_unique<Timer>();
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!windowManager->Initialize(hInstance, 1280, 720, L"DirectX 11 Engine"))
        return false;

    windowManager->SetResizeCallback([this](UINT width, UINT height)
                                     { this->OnResize(width, height); });

    windowManager->SetMessageCallback([this](UINT message, WPARAM wParam, LPARAM lParam)
                                      { this->ProcessMessage(message, wParam, lParam); });

    if (!InitializeSystems())
        return false;

    currentScene = std::make_unique<Scene>(registry, "Main Scene");
    currentScene->Initialize(renderSystem->GetMeshManager(), renderSystem->GetResourceManager());
    CreateScene();

    timer->Reset();
    timer->Start();

    return true;
}

bool Application::InitializeSystems()
{
    HWND hwnd = windowManager->GetWindowHandle();
    UINT width = windowManager->GetWidth();
    UINT height = windowManager->GetHeight();

    renderSystem = std::make_unique<RenderSystem>(registry, hwnd, width, height, *timer);
    if (!renderSystem->Initialize())
        return false;

    inputSystem = std::make_unique<InputSystem>(registry, hwnd);
    if (!inputSystem->Initialize())
        return false;

    return true;
}

void Application::CreateScene()
{
    EntityID cameraEntity = currentScene->CreateCamera({0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f});

    currentScene->CreateCube({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
    currentScene->CreateCube({2.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
    currentScene->CreateSphere(1.0f, 20, 20, {0.0f, 1.0f, 5.0f}, {1.0f, 1.0f, 1.0f});
    currentScene->CreateSphere(1.0f, 20, 20, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
    // removed ground plane until i create procedural grid for it

    EntityID lightEntity = currentScene->CreateLight({0.0f, -1.0f, 0.0f}, {0.9f, 0.9f, 0.9f, 1.0f});

    if (auto *transform = registry.GetComponent<TransformComponent>(lightEntity))
    {
        transform->position = {0.0f, 3.0f, -3.0f};
    }
}

void Application::Run()
{
    MSG msg = {};

    while (isRunning)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                isRunning = false;
                break;
            }
        }

        if (!isRunning)
            break;

        timer->Tick();
        float deltaTime = timer->GetDeltaTime();

        inputSystem->Update(deltaTime);
        renderSystem->Update(deltaTime);

        renderSystem->Render();
    }
}

void Application::Shutdown()
{
    // default
}

void Application::OnResize(UINT width, UINT height)
{
    if (renderSystem)
    {
        renderSystem->OnResize(width, height);
    }
}

void Application::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND hwnd = windowManager->GetWindowHandle();
    if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        return;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        isRunning = false;
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
        if (inputSystem)
        {
            inputSystem->ProcessMessage(message, wParam, lParam);
        }
        break;
    }
}