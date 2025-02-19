# DirectX 11 Renderer

A DirectX 11-based 3D graphics renderer built in C++. Originally based on [Braynzar Soft’s DX11 tutorials](https://www.braynzarsoft.net/viewtutorial/q16390-braynzar-soft-directx-11-tutorials), this project has evolved into a more structured and modern DirectX engine.

---

## Setup & Installation

### Prerequisites

Before running the project, ensure you have:

- Windows 10/11
- Visual Studio (or the Build Tools)
- Windows SDK (for DirectX and shader compilation)
- Git

### Cloning the Repository

```sh
git clone https://github.com/creacher4/DirectXRenderer.git
cd DirectXRenderer
```

---

## Running the Renderer

### Using the Precompiled Executable

Navigate to the `build/` directory and run the executable:

```sh
cd build
DX11App.exe
```

---

## Updating the Project

To update your local repository with the latest version:

```sh
git pull origin main
```

To push your changes:

```sh
git add .
git commit -m "Updated DX11 renderer with camera and rotating cubes"
git push origin main
```

---

## Current Progress

- Refactored to an object-oriented structure
- Rewrote DX11 initialization into a structured class
- Added depth buffering
- Created a render of two rotating cubes

---

## Upcoming Features

- Model Loading (OBJ, FBX support)
- Lighting (Phong, point, and directional lights)
- Texture Mapping (loading and sampling textures)
- Advanced camera system (WASD + mouse look)
- Game loop optimizations & FPS locking

---

## Credits

Based on Braynzar Soft’s DX11 tutorials. Originally developed for university coursework, now evolving into a standalone project.

---

## License

This project is for educational purposes as of now. No commercial use is intended.
