# Gravity Sim
A lightweight 2D Newtonian gravity simulation built with C++17 and OpenGL.

## Key Features
- **N-Body Simulation:** Real-time gravity calculations for multiple objects with custom mass and velocity. Note: in order to change the simulation, the main function source code needs to be modified.
- **CMake-based Build System:** Made and tested on Linux, Windows support yet implemented in CMake but untested.
- **Interactive Camera:** Full control over zoom and movement in the simulation space.

## Controls
| Action | Key |
| :--- | :--- |
| **Move Camera** | `Arrow Keys` |
| **Zoom In/Out** | `+` / `-` |
| **Simulation Speed** | `Z` / `X` |

## Requirements
- **Compiler:** GCC/G++ min. v9 (with C++17 support)
- **Build System:** CMake min. v3.28.3
- **Graphics:** OpenGL 3.3+ compatible hardware

## Dependencies
- **GLM**: [https://github.com/g-truc/glm] (included in the repository)
- **GLAD**: [https://glad.dav1d.de/] (included in the repository)
- **GLFW3**: [https://www.glfw.org/] (needs to be manually installed)

## GLFW installation guide
Debian/Ubuntu:
```
sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
```

## Compilation 
```
mkdir build
cd build
cmake ..
make
./gravity-sim
```
