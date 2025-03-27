#pragma once

#include <windows.h>
#include <string>
#include <functional>

// documentation coming soon i promise
class WindowManager
{
public:
    WindowManager();
    ~WindowManager();

    bool Initialize(HINSTANCE hInstance, int width, int height, const std::wstring &title);
    HWND GetWindowHandle() const { return hwnd; }

    void SetResizeCallback(std::function<void(UINT, UINT)> callback) { resizeCallback = callback; }
    void SetMessageCallback(std::function<void(UINT, WPARAM, LPARAM)> callback) { messageCallback = callback; }

    int GetWidth() const { return windowWidth; }
    int GetHeight() const { return windowHeight; }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND hwnd;
    HINSTANCE hInstance;
    int windowWidth;
    int windowHeight;
    std::wstring windowTitle;

    std::function<void(UINT, UINT)> resizeCallback;
    std::function<void(UINT, WPARAM, LPARAM)> messageCallback;
};