# DirectX 11 Renderer

A DirectX 11-based 3D graphics renderer built in C++.

## 📌 Features
- Object-oriented DirectX 11 setup
- Depth buffering & transformations
- Dynamic camera movement (WASD + mouse look)
- Texture mapping (using DirectX Tool Kit)
- HUD elmeents (FPS counter + camera position)
- Wireframe/solid rendering toggle

## 🚀 Setup & Build
### **Requirements**
- Windows 10/11
- Visual Studio (or Build Tools)
- Windows SDK
- Git
- CMake

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
- Fix texture remapping issues
- Implement a lighting system (Phong lighting, point lights etc.)
- Optimize rendering loop & FPS locking issues

## Credits

Originally based on [Braynzar Soft’s DX11 tutorials](https://www.braynzarsoft.net/viewtutorial/q16390-braynzar-soft-directx-11-tutorials) and initially built for University coursework, this engine has evolved into a more structured and documented project.

## License

This project is for educational purposes as of now. No commercial use is intended.
