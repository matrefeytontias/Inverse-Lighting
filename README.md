# Inverse lighting

This project aims at implementing the article ["Illumination Brush"](https://ieeexplore.ieee.org/xpls/icp.jsp?arnumber=4392727)
by Takeo Igarashi et al. This workflow uses GLFW with OpenGL 3.3, ImGui, Eigen and TinyGLTF.

This is based off work done by Félix Béasse, Camille Duquennoy, Iann Voltz and me
at Télécom ParisTech in the year 2018.

### Building on Windows

Download MSYS2 from the official website ; when that's done, install `gcc` and `make`.
Go in the top-level folder and run `make` to compile or `make run` to compile and run.
Easy like that.

### Building on Linux

Install the package `libglfw3-dev`, and run `make` to compile or `make run` to
compile and run. Even easier !
