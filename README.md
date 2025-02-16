# Graphics Rendering Engine

This is a **DirectX 11-based graphics rendering engine** built in C++.

## 📂 Project Setup

Before running the project, ensure that you have **Visual Studio Build Tools** and the **Windows SDK** installed.

### 🔧 Opening the Project in VS Code

1. Open **VS Code**.
2. Navigate to the project directory by opening the terminal (`Ctrl + ~`) and running:

```sh
cd "C:\Users\bluep\Desktop\random bs\WORK\.graphics rendering project\graphics rendering engine"
```

### 🛠 Setting Up the Build Environment

To ensure that **MSVC compiler** tools are available in the terminal, execute:

```sh
cmd /k "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

This sets up the required environment variables for compilation.

---

## 🎨 Compiling Shader Files

The engine requires **compiled shader object (CSO) files** for rendering. To compile the shaders, run:

```sh
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" /T vs_5_0 /E VSMain /Fo shaders/VertexShader.cso shaders/vertexShader.hlsl
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe" /T ps_5_0 /E PSMain /Fo shaders/PixelShader.cso shaders/pixelShader.hlsl
```

These commands:

- **Compile the vertex shader (`vertexShader.hlsl`)** into `VertexShader.cso`
- **Compile the pixel shader (`pixelShader.hlsl`)** into `PixelShader.cso`

---

## 🔨 Compiling the Main Application

Once the shaders are compiled, compile the main source file:

```sh
cl src\main.cpp /EHsc /Zi /MD /Fe:MyDX11App.exe /link /subsystem:windows user32.lib gdi32.lib d3d11.lib dxgi.lib d3dcompiler.lib
```

### Explanation:

- **`cl src\main.cpp`** → Compiles the `main.cpp` file.
- **`/EHsc`** → Enables standard exception handling.
- **`/Zi`** → Generates debugging information.
- **`/MD`** → Links against the multi-threaded runtime.
- **`/Fe:MyDX11App.exe`** → Specifies the output executable name.
- **`/link`** → Begins linker options.
- **`/subsystem:windows`** → Sets it as a Windows GUI application.
- **Libraries**: `user32.lib`, `gdi32.lib`, `d3d11.lib`, `dxgi.lib`, `d3dcompiler.lib` are linked for DirectX 11 rendering.

---

## ▶️ Running the Application

After compilation, run the application with:

```sh
MyDX11App.exe
```

---

## 📝 Notes

- If you encounter **compilation errors**, ensure that:
  - The **Windows SDK version** matches the one specified in the shader compilation path.
  - You have correctly set up **Visual Studio Build Tools**.
- If the shaders fail to compile, check the paths and ensure the **fxc.exe** tool is installed.

---

## 📜 License

This project is for educational purposes. Feel free to modify and extend it!
