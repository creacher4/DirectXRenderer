@echo off
echo ==============================
echo  Building DirectX Renderer
echo ==============================

:: Navigate to project root
cd /d "%~dp0"

:: Verify Visual Studio Build Tools Path
if not exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    echo [ERROR] Visual Studio Build Tools not found. Check your installation.
    exit /b 1
)

:: Set up Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

:: Verify fxc.exe exists
if not exist "C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" (
    echo [ERROR] Shader compiler (fxc.exe) not found. Check Windows SDK installation.
    exit /b 1
)

:: Compile vertex shader
echo Compiling Vertex Shader...
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" /T vs_5_0 /E VSMain /Fo shaders/VertexShader.cso shaders/vertexShader.hlsl
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile vertex shader.
    exit /b 1
)

:: Compile pixel shader
echo Compiling Pixel Shader...
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" /T ps_5_0 /E PSMain /Fo shaders/PixelShader.cso shaders/pixelShader.hlsl
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile pixel shader.
    exit /b 1
)

:: Ensure build directory exists
if not exist build mkdir build

:: Compile main application
echo Compiling Main Application...
cl src\main.cpp /Fe:build/MyDX11App.exe /link user32.lib gdi32.lib d3d11.lib dxgi.lib d3dcompiler.lib
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed.
    exit /b 1
)

:: Run the application
echo Running the application...
build/MyDX11App.exe
