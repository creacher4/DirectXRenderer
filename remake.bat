@echo off

cmake --build build
echo.
echo Build completed. Running the program...
.\build\bin\Debug\DX11Engine.exe
