## 12th February, 2025 [v0.1]

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

## 14th February, 2025 [v0.2]

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

## 19th February, 2025 [v0.3]

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

- Added high resolution timer to keep animations fps independant.
- Added dynamic HUD text (FPS + camera position) with SpriteBatch and SpriteFont.
  - Reapplied pipeline states since SpriteBatch was changing them.
- Updated inline comments.
  - Comments are now more thorough and informative.
- Added a custom depth-stencil state.

---

## 25th February, 2025 [v0.4]

- Refactored `main` into well-defined helper functions.
  - Separated responsibilities for window creation, device initialization, rendering, and input handling.
  - Broke up the monolithic structure into distinct initialization, update, and render stages.
- Added full window resizing support via an `OnResize` function.
  - The render target view and depth/stencil view are now recreated on resize.
  - Viewport and projection matrix are updated dynamically to match the new window size.
  - Window dimensions are displayed on the HUD (alongside FPS and camera position).
- Fixed a bug where after resizing the window, the depth buffer would fail.
  - Added a `ResetRenderStates()` helper function to reapply all pipeline states after a resize.
  - This ensures proper usage of the depth buffer so that near and far objects occlude each other correctly.
- Replaced raw COM pointers with `ComPtr` for automatic resource management.
- Heavily improved overall code organization and readability.
  - Enhanced error checking and maintainability.
- Added a simple directional lighting system.
- Fixed a bug where the texture map wouldn't render properly on the cubes.
  - The `png` file now renders properly on each cube face.
- Removed most, if not all, comments and renamed variables for better readability.
- Removed the use of global namespaces, introduced them locally when I felt like they were needed (personal preference).

---

## 15th March, 2025 [v0.6]

- Complete engine refactoring from a monolithic `main.cpp` to a modular ECS-based architecture.
  - Entity manager (`Registry` class).
  - Core (`Timer` and `WindowManager` classes).
  - Systems (`InputSystem` and `RenderSystem` class).
  - Managers created to reduce the size of the `RenderSystem` class and more easily separate concerns.
  - Other files created to implement the single responsibility principle (though badly at the moment).
- Added an `External` folder to keep the program self contained.
  - Original `DirectXTK`.
  - Removed the use of `SpriteBatch/SpriteFont` to render UI on the screen.
  - New `ImGui`.
  - Created a `.gitmodules` file.
- Rewrote `CMakeLists.txt` to reflect the previous changes.
  - Including directories via the `External` folder.
  - Defining `UNICODE` universally.
  - Renamed the executable.
- Created batch files to help streamline the development.
  - `build.bat`: Use this for when you add/delete files and need to rebuild the entire program using CMake.
  - `run.bat`: Use this to run the executable.
  - `remake.bat`: Use this for when you edit a file and need to run CMake.
- Implemented normal mapping for detailed surface rendering (Thank you [Polyhaven](https://polyhaven.com/) for providing textures etc.).
- Enhanced lighting system with support for three light types:
  - Directional lights.
  - Point lights with range-based attenuation.
  - Spot lights.
  - For now, you can only switch between them and some of the controls don't work.
- Added light intensity parameter for better lighting control.
- Added support for multiple textures (diffuse and normal maps).
- Improved material properties with proper tangent space calculations.
- Enhanced shader pipeline.
  - Refactored the shaders to add calculations for separate lighting systems.
  - Added shader support for normal mapping.
- Refined camera controls (sorta).
- Created a GUI with `ImGui`.
- Removed crashout comments and instead added comments to explain where the codebase needs improving.
- Remade the `.gitignore` file to keep the executable available, in case there are issues with rebuilding.
- Created a `NOTE.md` file.
- Deleted the old `Arial.spritefont` and `crying_rat.png` files.
- Added more textures to play with.
  - Unfortunately, I can't commit them at the moment since they're too large. I will fix this later.

---

## 16th March, 2025

- Fixed a bug where sphere mesh would be created inside out, causing weird texture and viewport issues.
  - Added pole caps to the sphere mesh.
- Added `Assimp` as a submodule for future use.

---

## 17th March, 2025

- Replaced the old shading system with a new Lambertian-based shader.
- Updated parts of the documentation (yet to publish).
- Updated the scene.
  - Removed the ground plane until I can add a compute shader for the procedural grid texture.
- Added a 'Known Bugs' section in the changelog.
- Updated sphere mesh creation method.
  - Now properly calculates more accurate tangent vectors.
  - This means slightly better normal mapping support.
- Added a check in the `MeshManager::CalculateTangentBitangent` function to deal with division by zero.
- Updated cube mesh creation method.
  - Now dynamically computes and assigns tangents/bitangents for each face in a single loop.

---

## 18th March, 2025

- Some small changes to the component system.
  - Each entity works with different properties and so, the component system is being updated to better support this.
  - An improved lighting component for better lighting type support.
  - A new camera component that handles camera movement separately from other entities.
- Updated the rest of the codebase to reflect these changes.
  - Updated the `Scene` class methods to use the new component system.
- Updated the GUI to reflect the new component system, bettering the user experience.
- Updated formatting of some code, specifically in the `Application` class for better readability.
- Added some documentation through comments.
  - Which is a lot easier now, since I feel like I'm making actual productive progress on the project.
- Made plans to update the pixel shader to support an enhanced lighting system, after finding a few bugs.
  - Also means, the (maybe) beginnings of a compute shader for (specifically) the procedural grid textures I've been planning for a while.
- Added `ImGuizmo` as a submodule for future use.

---

## 20th March, 2025

- Changed the scene setup for better mesh/lighting visualisations
- Improved the pixel shader (somewhat. I quite liked the previous version and so might revert back to it in some manner).
  - Improved normal mapping calculations.
  - Enhanced the lighting model with a new roughness calculation (in preparation for PBR in the future).
  - Point lights:
    - Improved distance attenuation with smoother falloff.
  - Spotlights:
    - More accurate cone calculations with smoother transitions.
    - Improved attenuation at spotlight edges.
  - Added view angle dependancy for a more natural rim effect.
  - Added some optional post-processing effects (commented out for now).
- Found a new bug with the directional light setup.

---

## Known Bugs

- When changing fov to 0, the program crashes with an assertion failure.
  - This means you can't cross out the entire number to write a custom fov, but instead have to edit the individual digits.
- The directional light seems like it emits the light from the origin (0, 0) and so anything at the origin will remain either unlit or badly lit.
  - There are a few other issues that come along with this but I'm yet to document them all.

---

## Future Plans

- NOTE: A lot of the plans I want to take to make this engine better are written within the codebase itself. Also, the engine is not feature complete yet. Both for my taste and for my university assignment.
- Adding documentation either via `Doxygen` or `GitBook`.
- Adding model loading via `Assimp`.
- Supporting multiple cameras or a dedicated 'main' camera.
- Moving movement logic to a separate controller.
- Combine multiple lights rather than using the first light entity.
  - Essentially, creating more light entities rather than splitting light types between one light entity.
- Possibly splitting pipeline manager's responsibilities further (into a state manager, a buffer manager, and a draw call manager for example).
- Isolating utility functions (like the tangent/bitangent calculations) into a maths class or similar.
- Split entity management into a dedicated `EntityManager` class to further decouple entity lifecycles from component storage.
- Learn about cache-friendly structures such as sparse sets.
- Set up an event or message-passing system.
- Some other stuff I'm probably forgetting but writing this here as a note to myself in case someone attacks me for it.
