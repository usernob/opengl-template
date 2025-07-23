## opengl template
Template repository for creating opengl project.

### What included
- [GLFW](https://github.com/glfw/glfw.git) for cross platfrom windowing
- GLAD for opengl loader
- [GLM](https://github.com/g-truc/glm.git) for math library
- [Dear IMGUI](https://github.com/ocornut/imgui) for simple debugging gui
- [ASSIMP](https://github.com/assimp/assimp.git) for model loader

### Requirement
- Cmake
- C compiler(gcc, clang, etc)
- Ninja 

### Building
```sh
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug  --fresh
```
after that
```sh
cmake --build build --config Debug
```
