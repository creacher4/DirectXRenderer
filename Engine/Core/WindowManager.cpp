#include "WindowManager.h"

WindowManager::WindowManager()
    : hwnd(NULL), hInstance(NULL), windowWidth(0), windowHeight(0)
{
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

    // register window class
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

    // associate this instance with the window handle using GWLP_USERDATA
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return true;
}

LRESULT CALLBACK WindowManager::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // retrieve instance pointer stored in GWLP_USERDATA
    WindowManager *self = reinterpret_cast<WindowManager *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (self)
    {
        if (message == WM_SIZE && wParam != SIZE_MINIMIZED)
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            self->windowWidth = width;
            self->windowHeight = height;
            if (self->resizeCallback && width > 0 && height > 0)
                self->resizeCallback(width, height);
        }
        if (self->messageCallback)
            self->messageCallback(message, wParam, lParam);
    }
    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}