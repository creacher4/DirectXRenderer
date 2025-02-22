## 12th February, 2025

- Installed DirectX SDK and set up the development environment.
- Followed Direct3D documentation and tutorials to establish the rendering pipeline.
- Created an initial `main.cpp` that opened a Win32 window and rendered a black triangle on a dark blue background.
- Debugged DirectX setup and issues.

---

## 13th February, 2025

- Began HLSL shader development.
- Researched shader programming via Microsoft Docs and related YouTube tutorials.
- Reviewed linear algebra concepts for shader transformations.

---

## 14th February, 2025

- Restarted the rendering system using a structured tutorial approach.
- Achieved the following rendering milestones:
  - Rendered a black triangle on a blue background.
  - Rendered an RGB triangle.
  - Implemented indexed rendering for an RGB square.
- Split shader code into separate files (`vertexShader.hlsl` and `pixelShader.hlsl`), ensuring proper color interpolation and transformation handling.

---

## 15th February, 2025

- Implemented a static camera positioned at (0, 3, -8) with a correct perspective projection.
- Integrated depth testing (Z-buffer) to handle render order.
- Applied transformation matrices to two cubes:
  - Implemented independent rotations and spatial positioning.
- Optimized shader constant buffers for efficient transformation updates.

---

## 17th February, 2025

- Made initial attempts to refactor the monolithic `main.cpp` into distinct modular classes (e.g., `Renderer` and `Camera`).
- Encountered object initialization and dependency ordering issues.

---

## 19th February, 2025

- Reverted to a simpler rendering approach, keeping everything in `main`:
  - Restored basic triangle and rectangle rendering.
  - Reintegrated animated cube rendering (y-axis and x-axis rotations).
- Improved code readability with structured comments.

---

## 20th February, 2025

- Added render state toggling:
  - Pressing "R" switches between wireframe and solid rasterizer states.
- Implemented a dynamic camera system:
  - Enabled movement using WASD keys based on camera orientation.

---

## 21st February, 2025

- Integrated DirectX Tool Kit (DXTK) using vcpkg.
- Created a CMakeLists.txt file for easier rebuild.
- Created a `.gitignore` to exclude unnecessary files.
- Implemented texture mapping via the DirectXTK.
- Updated README.md and removed troubleshooting section.
- Tried to fix a bug with CMake that caused configuration to fail.

---

## 22nd February, 2025

- Added high resolution timer to keep animations fps independant
- Added dynamic HUD text (FPS + camera position) with SpriteBatch and SpriteFont.
  - Reapplied pipeline states since SpriteBatch was changing them.
- Updated inline comments.
  - Comments are now more thorough and informative.
- Added a custom depth-stencil state.

## By 25th February, 2025

- Improve project documentation (in progress).
- Fix the texture mapping (in progress).
- Setup Basic lighting techniques (Phong lighting, point lights).
- Refactor resource management to use smart pointers (e.g., ComPtr) for COM objects.
- Add new repo issue about CMake + compilation (unless solved).
- Other error handling improvements and pipeline state optimizations to be determined.
