![DYE_logo_outlined](https://user-images.githubusercontent.com/8101387/219255174-fdb29aa7-6c08-42ca-9365-a86e0d774d94.png)
# DYEngine
DYEngine is a WIP 2D game engine (with the possibility of 3D rendering).

## Modules
DYEngine is divided into 2 sub-project modules:
* **DYEngine**: The core of the game engine. It includes everything you need to make a code-only game.
* **DYEditor**: It provides the ECS object model using entt & an editor on top of that for editting scenes & entities. It comes with a code generation tool for generating type information of user-defined components & systems for editor type registry. The module is still in an very early stage.

## Environment Setup
### Not really a guide, just how I normally use this engine. Most of the following tools come with CLion as bundles
- **IDE**: CLion (CMake 3.17+)
- **Tested Compilers**: 
  - GNU 11.2.0
  - MSVC 19.34.31937.0
  - Clang 15.0.1

Right now I am using CLion to configure & build the project.
In order to support multiple compilers, I had to include two versions of SDL2: MinGW & MSVC versions, which I downloaded from [SDL2 repository](https://github.com/libsdl-org/SDL/releases).  
I did some reorganizations to the SDL2 folders so whether you use MinGW or MSVC (both cl.exe and clang-cl.exe) to compile, the build system will still be able to find the correct version of library to link.

## Library Dependencies (included in the project)
### DYEngine
- **SDL2** (2.0.12)
- **glad**
- **glm**
- **stb_image**
- **Dear ImGui**
- **ImGuizmo**
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

## Resources
It wouldn't have been possible to implement this project without the help of the internet!  
Here are some resources that I use to make it happen! The list is non-exhaustive of course:  
* **Hazel engine**: https://github.com/TheCherno/Hazel
* **Halley engine**: https://github.com/amzeratul/halley
* ...and a lot more to be added.
