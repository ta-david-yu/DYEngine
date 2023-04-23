![DYE_logo_outlined](https://user-images.githubusercontent.com/8101387/219255174-fdb29aa7-6c08-42ca-9365-a86e0d774d94.png)
# DYEngine
DYEngine is a WIP 2D game engine (with the possibility of 3D rendering).

## Modules
DYEngine is divided into 2 sub-project modules:
* **DYEngine**: The core of the game engine. It includes everything you need to make a code-only game.
* **DYEditor**: It provides the ECS object model using entt & an editor on top of that for editting scenes & entities. It comes with a code generation tool for generating type information of user-defined components & systems for editor type registry. The module is still in an very early stage.

## Environment Setup
### Most of the following tools come with CLion as bundles
- **IDE**: CLion (CMake)
- **Build system**: Ninja
- **Compiler**: GCC (mingw) for C++20

Right now I am using CLion + GCC with Mingw to configure & build the project, haven't tested it with other compiler (i.e. MSVC) yet.  
To achieve MSVC build, you probably have to replace SDL2 library with the MSVC version (the one included in the repository is mingw version).
You can download **SDL2-devel-2.0.12-VC.zip** in the release page of [SDL2 repository](https://github.com/libsdl-org/SDL/releases).

## Library Dependencies (included in the project)
### DYEngine
- **SDL2** (2.0.12)
- **glad**
- **glm**
- **stb_image**
- **Dear ImGui**
### DYEditor
- **entt**: use as the main object model in DYEngine. I might implement my own ECS framework if time allows it.
- **tomlplusplus**: most of the custom text-based files in DYEditor will be stored as toml (i.e. scene/level files, asset meta files).
### DYEditor Code Generator
- **fmt**: for easier and more efficient source code generation process. Will replace it with c++ std::format library once it's widely supported by different compilers. 
### Pending (to include/included but not used)
- **OpenAL-Soft**: audio system



## Showcase
The project is still in a very early stage, but I am constantly working on it. 
I've also made a small tech demo with DYEngine, which you could check it out here - [DYE-Tech-Demo](https://github.com/ta-david-yu/DYE-Tech-Demo).   
Here is a short gameplay video of it - [DYE Tech Demo - Gameplay Youtube Video](https://youtu.be/0tD2ZouCuN4).  
**Warning**: the code was made within 1 week (I was crunching on it for the goal of showcasing it at our end-of-semester showcase party), so it is very dirty/unorganized. 

Here are some other WIP screenshots/footage:

**DYEditor (WIP)**  

https://user-images.githubusercontent.com/8101387/233865629-02971b03-32ef-4d06-b084-c146d34cc808.mp4

**Multi-window System**  

https://user-images.githubusercontent.com/8101387/211428433-88de9dfb-1df6-47c5-b2f2-73d366730937.mp4

**Collision detection & Debug drawing**  

https://user-images.githubusercontent.com/8101387/211428722-d59a1727-b169-4b57-84ad-aef03e2e87a4.mp4
