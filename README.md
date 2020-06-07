

# VCL

- [Introduction](#Introduction)
- [Compile the library](#Compilation)


<a name="Introduction"></a>
## Introduction


VCL - Visual Computing Library - is a simple lightweight library on top of OpenGL provided to ease learning of 3D programming while avoiding re-coding everything from scratch (matrices, mesh structures, etc.). The library provides several helper structure and functions to set up 3D scene with interactive and/or animated elements.


The objective of the library is to be simple as simple as possible to read and use.
The majority of VCL structures and functions are aimed to be minimalistic without hidden states. The code remains fully compatible with direct raw OpenGL calls and user-defined shaders. The animation loop itself is entirely defined by the user.


The code contains two main parts:
* The VCL library itself in `vcl/` directory - contains the helper functions and structures
* The 3D scenes part of the Projet INF443 in `scenes/` directory. This project consists in one unique scene in the directory `scenes/main_scene/`, divided into one main file `main_scene.hpp/cpp`, and auxiliary files in the subdirectory `scenes/main_scene/parts/`. Each feature of the main scene is implemented in a specific file from that subdirectory, and `main_scene.hpp/cpp` is here to centralize the calls to all the auxiliary functions.


<a name="Compilation"></a>
## Compile the library



The compilation can be done either using the provided
* **Makefile**: Linux/MacOS only 
* **CMakeLists.txt** using CMake (Linux/MacOS/Windows)


The library has one external dependency: [GLFW](https://www.glfw.org/) which can be installed through standard packages in Linux/MacOS.


* To set up your system and compile, or edit the code, see the README from [drohmer/inf443_vcl](https://github.com/drohmer/inf443_vcl)