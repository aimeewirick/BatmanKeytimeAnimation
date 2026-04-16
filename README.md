# 🦇 Batman Keytime Animation

A real-time 3D animated scene built in **C++ and OpenGL**, featuring Lego Batman, a Gotham City cityscape, animated bat swarms, and a dramatic BatWing escape — all driven by a custom keytime animation system.

## 🎬 Demo

[![Batman Keytime Animation Demo](https://img.youtube.com/vi/gTZcyyYLg8s/maxresdefault.jpg)](https://www.youtube.com/watch?v=gTZcyyYLg8s)

> Click the image above to watch the full animation on YouTube.

---

## 🛠️ Built With

| Technology | Purpose |
|------------|---------|
| C++ | Core application logic and animation system |
| OpenGL / freeglut / GLEW | Real-time 3D rendering pipeline |
| GLSL | Custom vertex and fragment shaders |
| Assimp | 3D model (.obj) loading and parsing |
| GLM | Matrix and vector math (transformations, lighting) |
| Blender | 3D model editing and multi-object .obj consolidation |
| CMake | Cross-platform build configuration |

---

## ✨ Features

### 🦸 Lego Batman Character Animation
- Batman is composed of multiple separate `.obj` files, each representing a distinct color section (belt, torso, black bodysuit, etc.)
- Three distinct poses — **standing, left leg forward, right leg forward** — are switched via a custom keytime system to create a walking animation
- Batman disappears from the scene at a keytime-triggered moment as he boards the BatWing

### ⏱️ Custom Keytime Animation System
- Implemented a frame-accurate keytime system to orchestrate all scene events: character pose switching, bat swarm movement, Bat-Signal appearance, and BatWing departure
- Solved a non-trivial floating-point averaging issue in `GetValue()` — the keytime system returns averaged floats rather than discrete set values, requiring careful calibration of `AddTimeValue` inputs to produce reliable pose transitions at half-second intervals

### 🌆 Gotham City Cityscape
- Cityscape composed of a multi-object list: city geometry and separately rendered white light bulb objects for the building lights
- Background wall and road geometry built from hand-authored vertices and texture coordinates, with `.bmp` textures applied (asphalt, brick)
- Figuring out how to hand-place UV texture coordinates on custom geometry was one of the most rewarding technical challenges of the project

### 🦇 Bat Swarm
- Animated using a consolidated multi-bat `.obj` file created in Blender
- Bats loosely track Batman's position and fly off when he departs in the BatWing
- Future improvement: animate each bat independently for true swarm behavior

### 🔦 Bat-Signal
- 3D Bat-Signal `.obj` illuminated by a **spotlight** with a yellow circle added for visual emphasis
- Appearance triggered by a keytime value — synchronized with the overall scene narrative

### 🚗 BatWing Mobile
- Multi-part `.obj` file split into **black and red color sections** using separate material files
- Animated via keytime to depart the scene in sync with Batman's disappearance

---

## 🏗️ Architecture

```
BatmanKeytimeAnimation/
├── sample.cpp              # Main application entry point and render loop
├── keytime.cpp / .h        # Custom keytime animation system
├── loadobjfile.cpp         # .obj model loader
├── glslprogram.cpp / .h    # GLSL shader program management
├── vertexbufferobject.cpp  # VBO management
├── setlight.cpp            # Lighting configuration
├── setmaterial.cpp         # Material properties
├── bmptotexture.cpp        # BMP texture loading
├── osusphere/cone/torus    # Procedural geometry helpers
├── *.mtl                   # Material files for each character part
├── *.bmp / *.jpg           # Texture assets
└── glm/                    # GLM math library
```

---

## 🚀 Building & Running

### Prerequisites
- OpenGL-compatible GPU and drivers
- CMake 3.x+
- A C++17 compatible compiler (MSVC, GCC, or Clang)
- freeglut, GLEW (included or system-installed)

### Build (Windows / Visual Studio)
```bash
# Open Assimp.sln or Sample.sln in Visual Studio
# Set to Release or Debug
# Build and run
```

### Build (Linux/Mac with CMake)
```bash
mkdir build && cd build
cmake ..
make
./Sample
```

---

## 🎓 What I Learned

This project pushed me to develop a much deeper understanding of the OpenGL rendering pipeline — particularly around:

- **Multi-object scene management** — organizing complex scenes with many independently rendered objects, each with their own transforms, materials, and textures
- **Keytime animation** — understanding how interpolated float values work in animation systems, and how to design around their behavior rather than against it
- **Texture mapping** — hand-placing UV coordinates on custom geometry, which gave me a much more concrete understanding of how textures map to 3D surfaces
- **Scene narrative through code** — coordinating multiple animated elements (character, environment, swarm, vehicle) into a coherent story entirely through timing and transforms

---

## 👩‍💻 Author

**Aimee Wirick**  
Oregon State University — B.S. Computer Science, Expected June 2026  
[LinkedIn](https://www.linkedin.com/in/aimee-wirick-170765122) • [AimeeWirick.com](https://AimeeWirick.com) • [GitHub](https://github.com/aimeewirick)

