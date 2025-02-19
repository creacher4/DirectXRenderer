# 🎮 DirectX 11 Renderer

A **DirectX 11-based 3D graphics renderer** built in **C++**. Originally based on **Braynzar Soft’s DX11 tutorials**, now evolving into a more **structured and modern** DirectX engine.

---

## 🛠 **Setup & Installation**

### **🔹 Prerequisites**

Before running the project, ensure you have:

- ✅ **Windows 10/11** (Tested on Windows 11 Pro 24H2)
- ✅ **Visual Studio Build Tools** or **full Visual Studio**
- ✅ **Windows SDK** (for DirectX & shader compilation)
- ✅ **Git** (for version control)

### **📥 Cloning the Repository**

```sh
git clone https://github.com/creacher4/DirectXRenderer.git
cd DirectXRenderer
```

---

## 🚀 **Running the Renderer**

### 🏃 Run the Precompiled Executable

You **don't need to compile anything**! Simply navigate to the `build/` directory

```sh
cd build
```

and run

```sh
DX11App.exe
```

---

### 🛠 **Advanced Users / Manual Compilation**

If you want to modify or extend the renderer, you can manually compile it by following these steps:

#### **🛠 Shader Compilation (Optional)**

Use **FXC (Shader Compiler)**. It's optional because the shaders come precompiled (unless you change how the shaders work too).

```sh
fxc /T vs_5_0 /E main /Fo shaders/vertexShader.cso shaders/vertexShader.hlsl
fxc /T ps_5_0 /E main /Fo shaders/pixelShader.cso shaders/pixelShader.hlsl
```

#### **🛠 Compiling with MSVC (Command Line)**

```sh
cl /EHsc /Zi /MD /std:c++20 /O2 /Fo"build\" src\main.cpp /Fe"build\DX11App.exe" /link /subsystem:windows user32.lib gdi32.lib d3d11.lib dxgi.lib d3dcompiler.lib
```

---

## 🔄 **Updating the Project**

To get the latest version:

```sh
git pull origin main
```

To push your changes:

```sh
git add .
git commit -m "Updated DX11 OOP structure"
git push origin main
```

---

## ❓ **Troubleshooting**

🔴 **Shader Compilation Errors**  
✔ Ensure you have the **correct Windows SDK version**  
✔ Verify the `fxc.exe` path is correct (`C:\Program Files (x86)\Windows Kits\10\bin\x64\fxc.exe`)

🔴 **`cl.exe` Not Recognized**  
✔ Open **Developer Command Prompt** or manually set up the VS build environment:

```sh
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

---

## 🏗 **Current Progress**

✅ **Refactored to Object-Oriented Structure**  
✅ **Rewrote DX11 initialization into a structured class**  
✅ **Successfully rendering a colored triangle**

---

## 🔮 **Upcoming Features**

🚀 **Model Loading (OBJ, FBX Support)**  
💡 **Lighting (Phong, Point, Directional Lights)**  
🎨 **Texture Mapping (Loading & Sampling Textures)**  
🎥 **Camera System (WASD + Mouse Look)**  
🔄 **Game Loop Optimizations & FPS Locking**

---

## 📜 **Credits**

🔹 Based on **Braynzar Soft’s DX11 tutorials**  
🔹 Originally developed for **university coursework**, now evolving into a **standalone project**.

---

## 📌 **License**

This project is for **educational purposes only** (for now). No commercial use intended yet.
