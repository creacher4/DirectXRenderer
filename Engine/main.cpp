#include "Core/Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    Application app(hInstance);

    if (!app.Initialize())
    {
        MessageBox(NULL, L"Failed to initialize the application", L"Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    app.Run();
    return 0;
}