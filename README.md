![DYE_logo_outlined](https://user-images.githubusercontent.com/8101387/219255174-fdb29aa7-6c08-42ca-9365-a86e0d774d94.png)
# DYEngine
DYEngine is a WIP 2D game engine (with the possibility of 3D rendering).

## Modules
DYEngine is divided into several sub-projects:
* **DYEngine**: The core of the game engine. It includes everything you need to make a code-only game.
* **DYEntity** (WIP): The main object model of DYEngine is ECS-based and DYEntity provides that. For now it's essentially a wrapper around entt.
* **DYEditor** (WIP): It provides an editor UI on top of DYEngine & DYEntity to edit your worlds & entities. It comes with a code generation tool for generating runtime type information of user-defined components & systems. 

Both DYEntity & DYEditor are under implementation on branch: [feature/dyentity-dyeditor-implementation](https://github.com/ta-david-yu/DYEngine/tree/feature/dyentity-dyeditor-implementation)

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
### DYEntity
- **entt**: use as the main object model in DYEngine. I might implement my own ECS framework if time allows it.
### DYEditor Code Generator
- **fmt**: for easier and more efficient source code generation process. Will replace it with c++ std::format library once it's widely supported by different compilers. 
### Pending (to include/included but not used)
- **tomlplusplus**: most of the custom text-based files in DYEngine will be stored as toml (i.e. world/level files, asset meta files).
- **OpenAL-Soft**: audio system



## Showcase
The project is still in a very early stage, but I am constantly working on it. 
Right now the game engine doesn't have advanced features that one would imagine to have in an engine such as object model, rendering optimization or robust editor.

I've also made a small tech demo with DYEngine, which you could check it out here - [DYE-Tech-Demo](https://github.com/ta-david-yu/DYE-Tech-Demo).   
Here is a short gameplay video of it - [DYE Tech Demo - Gameplay Youtube Video](https://youtu.be/0tD2ZouCuN4).  
**Warning**: the code was made within 1 week (I was crunching on it for the goal of showcasing it at our end-of-semester showcase party), so it is very dirty/unorganized. 

Here are some other WIP screenshots/footage:

https://user-images.githubusercontent.com/8101387/211428378-24c91485-b080-486f-b750-cb40b5b0c1dd.mp4

https://user-images.githubusercontent.com/8101387/211428433-88de9dfb-1df6-47c5-b2f2-73d366730937.mp4

https://user-images.githubusercontent.com/8101387/211428722-d59a1727-b169-4b57-84ad-aef03e2e87a4.mp4
