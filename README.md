# 🎮 DirectX 11 Graphics Renderer

This is a **DirectX 11-based 3D graphics rendering engine** built in C++.  

---

## 🛠 **Setup & Installation**
Before running the project, ensure you have:
- **Windows OS** (Tested on Windows 11 Pro, version 24H2)
- **Visual Studio Build Tools** or **full Visual Studio**
- **Windows SDK** (for DirectX & shader compilation)
- **Git** (for version control)

### **1️⃣ Cloning the Repository**
If you haven’t cloned the repository yet, run:

```sh
git clone https://github.com/creacher4/DirectXRenderer.git
cd DirectXRenderer
```

---

## 🚀 **Building & Running the Project**
### **1️⃣ Using the Automated Script (Windows)**
For quick setup, run:

```sh
tools/build.bat
```

This will:
1. **Set up the Visual Studio build environment.**
2. **Compile the shaders (`.hlsl` → `.cso`).**
3. **Compile the main application (`MyDX11App.exe`).**
4. **Run the application.**

---

### **2️⃣ Manual Compilation**
If you prefer manual compilation, follow these steps:

#### **🔹 Set Up the Build Environment**
Run this command to configure MSVC (Microsoft’s compiler):

```sh
cmd /k "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

#### **🔹 Compile Shaders**
Compile the vertex and pixel shaders:

```sh
"C:\Path\To\fxc.exe" /T vs_5_0 /E VSMain /Fo shaders/VertexShader.cso shaders/vertexShader.hlsl
"C:\Path\To\fxc.exe" /T ps_5_0 /E PSMain /Fo shaders/PixelShader.cso shaders/pixelShader.hlsl
```

**Note:** Update `C:\Path\To\fxc.exe` to the correct location on your system.

#### **🔹 Compile the Main Application**
```sh
cl src\main.cpp /Fe:build/MyDX11App.exe /link user32.lib gdi32.lib d3d11.lib dxgi.lib d3dcompiler.lib
```

#### **🔹 Run the Application**
After compilation, the output executable will be located in `/build/`.  
To run it, use:
```sh
build/MyDX11App.exe
```

---

## 🔄 **Keeping the Project Updated**
To update your local repository with the latest changes, run:

```sh
git pull origin main
```

To commit and push new changes:

```sh
git add .
git commit -m "Updated shader compilation"
git push origin main
```

---

## ❓ **Troubleshooting**
- If **shader compilation fails**, ensure:
  - You have the correct **Windows SDK version**.
  - The `fxc.exe` path is correct.
- If **`cl` is not recognized**, make sure the **Visual Studio build environment is loaded**.

---

## 🔮 **Planned Features**
- 🌟 **Model loading support (OBJ, FBX)**
- 💡 **Lighting (Phong, Point Lights, Directional)**
- 🎨 **Texture mapping**
- 🎥 **Camera movement & controls**
- 🏎️ **Optimized rendering performance**

## 🔮 **Planned Architectural Changes**
- **Object-Oriented Programming (Encapsulation)**
- **Memory Management (Smart Pointers)**
- Other Additional changes

---

## 📜 **Credits**
This project follows a tutorial from **[Braynzar Soft](https://www.braynzarsoft.net/viewtutorial/q16390-braynzar-soft-directx-11-tutorials)**  
Originally developed for university coursework, this project has since evolved into an independent learning experiment focused on modernizing and improving DirectX 11 rendering techniques

---

## 📌 **License**
This project is for **educational purposes only** and follows a tutorial (for now). No commercial use intended (for now).
