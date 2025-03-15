cmake_minimum_required(VERSION 3.15)
project(DX11Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure runtime libraries are used in release mode
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
)

# define ImGui sources
file(GLOB IMGUI_SOURCES
    "${PROJECT_SOURCE_DIR}/External/imgui/*.cpp"
    "${PROJECT_SOURCE_DIR}/External/imgui/backends/imgui_impl_win32.cpp"
    "${PROJECT_SOURCE_DIR}/External/imgui/backends/imgui_impl_dx11.cpp"
)

# collect WICTextureLoader files from DirectXTK
file(GLOB DXTK_FILES
    "${PROJECT_SOURCE_DIR}/External/DirectXTK/Src/WICTextureLoader.cpp"
    "${PROJECT_SOURCE_DIR}/External/DirectXTK/Inc/WICTextureLoader.h"
)

add_executable(DX11Engine WIN32 ${SRC_FILES} ${IMGUI_SOURCES} ${DXTK_FILES})

target_include_directories(DX11Engine PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/External/imgui
    ${PROJECT_SOURCE_DIR}/External/imgui/backends
    ${PROJECT_SOURCE_DIR}/External/DirectXTK/Inc
)

# define UNICODE for the project
# target link libraries
target_compile_definitions(DX11Engine PRIVATE UNICODE _UNICODE)
target_link_libraries(DX11Engine d3d11 d3dcompiler dxgi)