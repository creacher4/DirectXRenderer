# DirectX 11 Renderer

A DirectX 11-based 3D graphics renderer built in C++.

## 📌 Features

- Object-oriented DirectX 11 setup
- Depth buffering & transformations
- Dynamic camera movement (WASD + mouse look)
- Texture mapping (using DirectX Tool Kit)
- HUD elements (FPS counter + camera position)
- Wireframe/solid rendering toggle
- Window resizing
- Simple directional lighting system

## 🚀 Setup & Build

### Requirements

- Windows 10/11
- Visual Studio (or Build Tools)
- Windows SDK
- Git
- CMake
- DirectX Tool Kit

### **Clone & Build**

```sh
git clone https://github.com/creacher4/DirectXRenderer.git
cd DirectXRenderer
mkdir build && cd build
cmake .. && cmake --build .
```

### **Run the Renderer**

```sh
.\build\Debug\DirectXRenderer.exe
```

## 🔄 Planned Updates

- Splitting `main.cpp` into additional classes/files (`WindowManager`, `Camera`, etc.)
- Advanced Lighting techniques (e.g., Phong, point lights)
- CMake + Compilation bugfixes (An issue I seem to be having when pulling from git)
- Other performance improvements to be determined

## Credits

Originally based on [Braynzar Soft’s DX11 tutorials](https://www.braynzarsoft.net/viewtutorial/q16390-braynzar-soft-directx-11-tutorials) and initially built for University coursework, this engine has evolved into a more structured and documented project.

## License

This project is for educational purposes as of now. No commercial use is intended.
