@echo off
setlocal

:: Setup VS and Windows SDK
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

:: Compile shaders
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" /T vs_5_0 /E VSMain /Fo shaders\VertexShader.cso shaders\shaders.hlsl
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" /T ps_5_0 /E PSMain /Fo shaders\PixelShader.cso shaders\shaders.hlsl

:: Compile main executable
cl src\main.cpp /link d3d11.lib user32.lib d3dcompiler.lib

:: Run the executable
main.exe


echo Build complete.
pause
