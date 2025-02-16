cmake_minimum_required(VERSION 3.10)
project(DirectXRenderer)

set(CMAKE_CXX_STANDARD 17)

add_executable(DirectXRenderer main.cpp renderer.cpp shaders.cpp)

target_link_libraries(DirectXRenderer d3d11 dxgi d3dcompiler user32)
