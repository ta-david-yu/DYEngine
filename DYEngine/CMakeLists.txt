project(DYEngine)

# Packages

# Define SDL2_PATH here that will be used by FindSDL2 module
set(SDL2_PATH "extern/SDL2")
find_package(SDL2 REQUIRED)

# DYEngine Sources / Headers
set(SOURCE_FILES
        src/Application.cpp
        src/WindowBase.cpp
        src/SDLWindow.cpp
        src/Time.cpp
        src/EventSystemBase.cpp
        src/SDLEventSystem.cpp src/ContextBase.cpp src/SDLContext.cpp)
set(HEADER_FILES
        include/Application.h
        include/WindowBase.h
        include/SDLWindow.h
        include/Time.h
        include/Events/Event.h
        include/Events/ApplicationEvent.h
        include/Input/KeyCode.h
        include/Events/KeyEvent.h
        include/Events/EventSystemBase.h
        include/Events/SDLEventSystem.h
        include/Base.h include/ContextBase.h include/SDLContext.h)

message(STATUS "Trace for CMAKE_CURRENT_SOURCE_DIR variable: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Trace for CMAKE_SOURCE_DIR variable: ${CMAKE_SOURCE_DIR}")

# External Library
add_subdirectory(extern/glad)
add_subdirectory(extern/imgui)

# Explicitly specify paths for SDL_gpu. It's hardcoded and ugly but easy.
#set(SDL2_GPU_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/SDL2_gpu/SDL2_gpu/release/mingw32/libSDL2_gpu.dll.a")
#set(SDL2_GPU_INCLUDE_PATH "extern/SDL2_gpu/include")

#include_directories(${SDL2_GPU_INCLUDE_PATH})

# Build DYEngine library
add_library(DYEngine STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES})


# Precompiled Header
target_precompile_headers(DYEngine PRIVATE Precompiled.h)

# Include Headers
target_include_directories(DYEngine PUBLIC include)
# SDL2_INCLUDE_DIR variable set by the FindSDL2 module (if it finds directory).
target_include_directories(DYEngine PRIVATE ${SDL2_INCLUDE_DIR})
target_include_directories(DYEngine PRIVATE extern/glad/include)
target_include_directories(DYEngine PUBLIC extern/imgui)
target_include_directories(DYEngine PRIVATE extern/imgui/examples)

# Link External Libraries with DYEngine lib
target_link_libraries(DYEngine
        #${SDL2MAIN_LIBRARY}
        ${SDL2_LIBRARY} # SDL2MAIN_LIBRARY is also specified within SDL2_LIBRARY by the FindSDL2 module. It's needed for Windows specific main function. If I don't use FindSDL2 module, I have to link it as well.
#        ${SDL2_GPU_LIBRARY_PATH}
#        SDL2_gpu
        glad
        imgui_sdl2)

