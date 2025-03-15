@echo off
if exist build (
    echo Removing previous build folder...
    rmdir /s /q build
)
mkdir build
echo Creating new build folder...
cd build
cmake ..
cd ..
cmake --build build
echo.
echo Build completed.

.\build\bin\Debug\DX11Engine.exe