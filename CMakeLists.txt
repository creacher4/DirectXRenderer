cmake_minimum_required(VERSION 3.10)

project(DirectXRenderer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find DirectX ToolKit
find_package(DirectXTK CONFIG REQUIRED)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# Add executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Link DirectX ToolKit
target_link_libraries(${PROJECT_NAME} Microsoft::DirectXTK)
