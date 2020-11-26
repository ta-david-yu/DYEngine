project(DYEngine)

# SDL2
set(SDL2_PATH "extern/SDL2")

# Packages
find_package(SDL2 REQUIRED)

# Includes

# SDL2_INCLUDE_DIR variable set by the FindSDL2 module (if it finds directory).
include_directories(${SDL2_INCLUDE_DIR})

# DYEngine Sources / Headers
set(SOURCE_FILES
        src/Application.cpp
        src/WindowBase.cpp
        src/SDLWindow.cpp
        src/Time.cpp src/EventSystemBase.cpp src/SDLEventSystem.cpp)
set(HEADER_FILES
        include/Application.h
        include/WindowBase.h
        include/SDLWindow.h
        include/Time.h
        include/Events/Event.h
        include/Events/ApplicationEvent.h include/Input/KeyCode.h include/Events/KeyEvent.h include/Events/EventSystemBase.h include/Events/SDLEventSystem.h include/Base.h)

# External Library
# Imgui
add_subdirectory(extern/imgui)

add_library(DYEngine STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Precompiled Header
target_precompile_headers(DYEngine PRIVATE Precompiled.h)

# Include Headers
target_include_directories(DYEngine PUBLIC include)
target_include_directories(DYEngine INTERFACE extern/SDL2/include)  # Should be hidden from the users

target_include_directories(DYEngine PUBLIC extern/imgui)
target_include_directories(DYEngine PUBLIC extern/imgui/examples)

# Link External Libraries with DYEngine lib
target_link_libraries(DYEngine
        #${SDL2MAIN_LIBRARY}
        ${SDL2_LIBRARY} # SDL2MAIN_LIBRARY is also specified within SDL2_LIBRARY by the FindSDL2 module. It's needed for Windows specific main function. If I don't use FindSDL2 module, I have to link it as well.
        imgui_sdl2)
