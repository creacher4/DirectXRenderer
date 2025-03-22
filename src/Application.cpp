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

// updating the formatting to easier read what each functions parameters are
void Application::CreateScene()
{
    // i forgot why im not using EntityID for everything in this function
    EntityID cameraEntity = currentScene->CreateCamera(
        {-0.867f, 2.304f, -1.181f}, // position
        {-0.298f, -0.144f, 0.944f}  // look direction
    );

    // maybe some instancing or something would be better here
    currentScene->CreateCube(
        {-1.8f, 2.3f, 4.1f}, // position
        {1.0f, 1.0f, 1.0f}   // scale
    );
    currentScene->CreateCube(
        {-3.0f, 1.0f, 3.9f}, // position
        {1.0f, 1.0f, 1.0f}   // scale
    );
    currentScene->CreateSphere(
        1.0f, 20, 20,          // radius, slices, stacks
        {-0.7f, 0.0f, 5.450f}, // position
        {1.0f, 1.0f, 1.0f}     // scale
    );
    currentScene->CreateSphere(
        1.0f, 20, 20,           // radius, slices, stacks
        {-4.5f, 2.428f, 4.29f}, // position
        {1.0f, 1.0f, 1.0f}      // scale
    );
    // removed ground plane until i create procedural grid for it

    /**
     * @brief - multiple lights work now. however, the ambient term for every light but the first one is ignored it seems
     *
     * @todo - add a way to add/remove lights within the editor
     */

    // some directional light (sun)
    currentScene->CreateLight(
        {0.0f, -1.0f, 0.0f},     // direction
        {0.7f, 0.7f, 0.7f, 1.0f} // color
    );

    // point light 1 (warm)
    currentScene->CreatePointLight(
        {-2.9f, 1.8f, 2.5f},      // position
        {1.0f, 0.8f, 0.6f, 1.0f}, // color
        7.5f                      // range
    );

    // point light 2 (cool)
    currentScene->CreatePointLight(
        {2.0f, 1.5f, 4.0f},       // position
        {0.6f, 0.8f, 1.0f, 1.0f}, // color (cool)
        5.0f                      // range
    );

    // currentScene->CreateSpotLight(
    //     {0.0f, 3.0f, -3.0f},      // position
    //     {0.0f, -1.0f, 0.0f},      // direction
    //     {0.2f, 0.2f, 0.9f, 1.0f}, // color
    //     10.0f);                   // range
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