#include "WindowManager.h"

// static instance for the window manager
// this is necessary for the window procedure to work
static WindowManager *s_pWindowManager = nullptr;

WindowManager::WindowManager()
    : hwnd(NULL), hInstance(NULL), windowWidth(0), windowHeight(0)
{
    s_pWindowManager = this;
}

WindowManager::~WindowManager()
{
}

bool WindowManager::Initialize(HINSTANCE hInstance, int width, int height, const std::wstring &title)
{
    this->hInstance = hInstance;
    this->windowWidth = width;
    this->windowHeight = height;
    this->windowTitle = title;

    // Register window class
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowManager::WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"DirectX11EngineWindow";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return false;

    // Create window
    RECT rc = {0, 0, (LONG)width, (LONG)height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindow(
        L"DirectX11EngineWindow",
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwnd)
        return false;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return true;
}

LRESULT CALLBACK WindowManager::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Forward message to the appropriate instance
    if (s_pWindowManager)
    {
        // Handle window resize
        if (message == WM_SIZE && wParam != SIZE_MINIMIZED)
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);

            // Update stored dimensions
            s_pWindowManager->windowWidth = width;
            s_pWindowManager->windowHeight = height;

            // Notify via callback
            if (s_pWindowManager->resizeCallback && width > 0 && height > 0)
            {
                s_pWindowManager->resizeCallback(width, height);
            }
        }

        // Forward all messages to the message callback
        if (s_pWindowManager->messageCallback)
        {
            s_pWindowManager->messageCallback(message, wParam, lParam);
        }
    }

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}