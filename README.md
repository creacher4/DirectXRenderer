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

### **2️⃣ Manual Compilation (Optional)**
If `tools/build.bat` does not work or you want to customize the build process, you can manually compile the project.

1. **Navigate to the project directory** (run `cd "Path\To\Directory\DirectXRenderer"`)
2. **Set up the MSVC build environment** (run `vcvars64.bat`).
3. **Compile shaders** using `fxc.exe`.
4. **Compile the main application** using `cl.exe`.
5. **Run the executable** located in `/build/`.

For exact compiler flags, see `tools/build.bat`.

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
- **Further structural improvements (TBD)**

---

## 📜 **Credits**
This project follows a tutorial from **[Braynzar Soft](https://www.braynzarsoft.net/viewtutorial/q16390-braynzar-soft-directx-11-tutorials)**. Originally developed for university coursework, this project has since evolved into an independent learning experiment focused on modernizing and improving DirectX 11 rendering techniques.

---

## 📌 **License**
This project is for **educational purposes only** and follows a tutorial (for now). No commercial use intended (for now).
