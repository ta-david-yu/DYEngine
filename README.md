# DYEngine
DYEngine is a WIP 2D game engine (with the possibility of 3D rendering)

# Environment Setup
Right now I am using CLion + gcc with Mingw to configure & build the project, haven't tested it with other compiler (i.e. MSVC) yet.  
To achieve MSVC build, you probably have to replace SDL2 library with the MSVC version (the one included in the repository is mingw version).
You can download **SDL2-devel-2.0.12-VC.zip** in the release page of [SDL2 repository](https://github.com/libsdl-org/SDL/releases).

# External Dependencies
- SDL2 (2.0.12, using OpenGL)
- glad
- glm
- stb_image
- Dear ImGui
- possibly rttr & entt in the future (under research).

# Showcase
The project is still in a very early stage, but I am constantly working on it. 
Right now the game engine doesn't have advanced features that one would imagine to have in an engine such as object model, rendering optimization or robust editor.

I've also made a small tech demo with DYEngine, which you could check it out here - [DYE-Tech-Demo](https://github.com/ta-david-yu/DYE-Tech-Demo).   
Here is a short gameplay video of it - [DYE Tech Demo - Gameplay Youtube Video](https://youtu.be/0tD2ZouCuN4).  
**Warning**: the code was made within 1 week (I was crunching on it for the goal of showcasing it at our end-of-semester showcase party), so it is very dirty/unorganized. 

Here are some other WIP screenshots/footage:

https://user-images.githubusercontent.com/8101387/211428378-24c91485-b080-486f-b750-cb40b5b0c1dd.mp4

https://user-images.githubusercontent.com/8101387/211428433-88de9dfb-1df6-47c5-b2f2-73d366730937.mp4

https://user-images.githubusercontent.com/8101387/211428722-d59a1727-b169-4b57-84ad-aef03e2e87a4.mp4
