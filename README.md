![DYE_logo_outlined](https://user-images.githubusercontent.com/8101387/219255174-fdb29aa7-6c08-42ca-9365-a86e0d774d94.png)
# DYEngine
DYEngine is a WIP 2D game engine (with the possibility of 3D rendering).

## Modules
DYEngine is divided into 2 sub-project modules:
* **DYEngine**: The core of the game engine. It includes everything you need to make a code-only game.
* **DYEditor**: It provides the ECS object model using entt & an editor on top of that for editting scenes & entities. It comes with a code generation tool for generating type information of user-defined components & systems for editor type registry. The module is still in an very early stage.

## Getting Started

Right now I am using CLion to configure & build the project. Therefore, building with CLion is the fastest and the most stable way to build the project.
In order to support multiple compilers, I had to include two versions of SDL2: MinGW & MSVC versions, which I downloaded from [SDL2 repository](https://github.com/libsdl-org/SDL/releases).  
I did some reorganizations to the SDL2 folders so whether you use MinGW or MSVC (both cl.exe and clang-cl.exe) to compile, the build system will still be able to find the correct version of library to link.

### Build with CLion
Most of the following tools come with CLion as bundles
- **IDE**: CLion (CMake 3.17+)
- **Tested Compilers**: 
  - GNU 11.2.0 (MinGW)
  - MSVC 19.34.31937.0
  - Clang 15.0.1 (MSVC compatible)

Once you have all of the above installed, you can build and run the Sandbox configuration in CLion directly.

### Build with CMake and Visual Studio
[How to CMake Good - 0c - Using Visual Studio](https://youtu.be/6aiV7Z9NRhk)

1. Use the root CMakeList.txt to generate a visual studio solution in the build folder.
    ```shell
    - cmake -G "Visual Studio 17 2022" -B "build"
    ```
2. Open the generated visual studio solution file.
3. Set `Sandbox_Editor` as the Startup Project.
4. Build.
5Run the executable and you will see the Sandbox app running properly.

### Build with CMake, Ninja (as the generator), and Visual C++ (cl.exe as the compiler) OR Clang with MSVC-like command-line (clang-cl.exe as the compiler)
[How to CMake Good - 0d - Visual C++ without Visual Studio](https://youtu.be/nGnKmEkNBkw)

Similar to the Visual Studio, but you don't have to open the solution file in the Visual Studio.
Everything can be done in the terminal.

1. Start the Visual Studio Developer Command Prompt in the terminal by running the vcvarsall.bat that's included the Visual Studio build tools.
    ```shell
    # The path could be different depending on where you install Visual Studio
    # Specify the target architecture as the first argument (x64 in our case).
    - "C:\<path-to-visual-studio>\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ```
2. Use the root CMakeList.txt to generate a visual studio solution in the build folder using Ninja as the generator and cl.exe as the c/cpp compiler:
    ```shell
    - cmake -G Ninja -B "build" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
    ```
    You can also use clang-cl as the compiler:
    ```shell
    - cmake -G Ninja -B "build" -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl
    ```
3. Build the project with the following command:
    ```shell
    - cd build
    - ninja Sandbox_Editor
    ```
4. Run the executable and you will see the Sandbox app running properly.

### Build with CMake and Visual C++ (cl.exe as the compiler)
[How to CMake Good - 0d - Visual C++ without Visual Studio](https://youtu.be/nGnKmEkNBkw)

Similar to the Visual Studio method, but you don't have to open the solution file in the Visual Studio.
Everything can be done in the terminal.

1. Start the Visual Studio Developer Command Prompt in the terminal by running the vcvarsall.bat that's included the Visual Studio build tools.
    ```shell
    # The path could be different depending on where you install Visual Studio
    # Specify the target architecture as the first argument (x64 in our case).
    - "C:\<path-to-visual-studio>\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ```
2. Use the root CMakeList.txt to generate a visual studio solution in the build folder with the following command:
    ```shell
    - cmake -G "Visual Studio 17 2022" -B "build"
    ```
3. Build the project with the following command:
    ```shell
    - cmake --build "build" --target Sandbox_Editor
    ```
4. Run the executable and you will see the Sandbox app running properly.

## Library Dependencies (included in the project)
### DYEngine
- **SDL2** (2.26.5)
- **glad**
- **glm**
- **stb_image**
- **Dear ImGui**
- **ImGuizmo**
- **raudio**
### DYEditor
- **EnTT**: use as the main object model in DYEngine. I might implement my own ECS framework if time allows it.
- **tomlplusplus**: most of the custom text-based files in DYEditor will be stored as toml (i.e. scene/level files, asset meta files).
### DYEditor Code Generator
- **fmt**: for easier and more efficient source code generation process. Will replace it with c++ std::format library once it's widely supported by different compilers. 
### Pending (to include/included but not used)
- **OpenAL-Soft**: Alternative for 3D audio. Need more research.

## Showcase
The project is still in a very early stage, but I am constantly working on it.  
Here are some videos that might help you get the "feel" of DYEngine:
  
**DYEngine & Editor Feature Highlights 2023**  
This video includes some of the features of DYEngine & DYEditor.  
[![Watch the video](https://img.youtube.com/vi/ZrwCcynemSA/maxresdefault.jpg)](https://youtu.be/ZrwCcynemSA)  

**Main Battle Turtle**  
I made the game with two of my friends (Anton Sagel & Trey Ramm) during 2023 CGL Turtle Game Jam.
It's a really good opportunity to test out the game engine.  
[![Watch the video](https://img.youtube.com/vi/CldpJoQgW-Q/maxresdefault.jpg)](https://youtu.be/CldpJoQgW-Q)  

**DYE Tech Demo**  
I've also made a small tech demo game collection with DYEngine, which you could check it out here - [DYE-Tech-Demo](https://github.com/ta-david-yu/DYE-Tech-Demo).  
**Warning**: the code was made within 1 week (I was crunching on it for the goal of showcasing it at our end-of-semester showcase party), so it is very dirty/unorganized.  
[![Watch the video](https://img.youtube.com/vi/0tD2ZouCuN4/maxresdefault.jpg)](https://youtu.be/0tD2ZouCuN4)  

## Resources
It wouldn't have been possible to implement this project without the help of the internet!  
Here are some resources that I use to make it happen! The list is non-exhaustive of course:  
* **Hazel engine**: https://github.com/TheCherno/Hazel
* **Halley engine**: https://github.com/amzeratul/halley
* ...and a lot more to be added.
