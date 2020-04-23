

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
* The 3D scenes part of the Projet INF443 in `scenes/` directory. Each scene is fully defined in its subdirectory, and the switch between different scene is set using a keyword defined in `scene/current_scene.hpp` file. The main scenes are in the subdirectory `scenes/main_scene/` while the other subdirectories contain codes for smaller scenes that were used to design separate items independently. As such, those smaller scenes will often execute a single item from the final scenes, and the code for the main scenes is more or less a merge of the codes for the smaller scenes.


<a name="Compilation"></a>
## Compile the library



The compilation can be done either using the provided
* **Makefile**: Linux/MacOS only 
* **CMakeLists.txt** using CMake (Linux/MacOS/Windows)


The library has one external dependency: [GLFW](https://www.glfw.org/) which can be installed through standard packages in Linux/MacOS (see the provided detailed tutorials).


* To set up your system and compile, or edit the code, see the README from [drohmer/inf443_vcl](https://github.com/drohmer/inf443_vcl)