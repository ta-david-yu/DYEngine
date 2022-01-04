project(DYEngine VERSION 0.0.1)

# ===========================================================================================
# DYEngine Sources / Headers
set(SOURCE_FILES
        src/Application.cpp
        src/WindowBase.cpp
        src/SDLWindow.cpp
        src/Time.cpp
        src/EventSystemBase.cpp
        src/SDLEventSystem.cpp
        src/ContextBase.cpp
        src/SDLContext.cpp
        src/LayerBase.cpp
        src/LayerStack.cpp
        src/ImGuiLayer.cpp
        src/Logger.cpp
        src/SceneLayer.cpp
        src/Entity.cpp
        src/ComponentBase.cpp
        src/Type.cpp
        src/Transform.cpp
        src/Shader.cpp
        src/OpenGL.cpp
        src/Buffer.cpp
        src/Renderer.cpp
        src/VertexArray.cpp
        src/RenderCommand.cpp
        src/ImageRenderer.cpp
        src/Texture.cpp)
set(HEADER_FILES
        include/Application.h
        include/WindowBase.h
        include/SDLWindow.h
        include/Time.h
        include/Event/Event.h
        include/Event/ApplicationEvent.h
        include/Input/KeyCode.h
        include/Event/KeyEvent.h
        include/Event/EventSystemBase.h
        include/Event/SDLEventSystem.h
        include/Base.h include/ContextBase.h
        include/SDLContext.h
        include/AppEntryPoint.h
        include/LayerBase.h
        include/LayerStack.h
        include/ImGuiLayer.h
        include/Logger.h
        include/Scene/SceneLayer.h
        include/Scene/Entity.h
        include/Scene/ComponentBase.h
        include/Util/Type.h
        include/Scene/Transform.h
        include/Graphics/Shader.h
        include/Graphics/OpenGL.h
        include/Graphics/Buffer.h
        include/Graphics/Renderer.h
        include/Graphics/VertexArray.h
        include/Graphics/RenderCommand.h
        include/Scene/ImageRenderer.h
        include/Graphics/Texture.h
        include/Event/MouseEvent.h
        include/Input/MouseCode.h
        include/Message/Messaging.h)

message(STATUS "DYEngine Source Files: ${SOURCE_FILES}")
message(STATUS "DYEngine Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External 3rd party library
# Define SDL2_PATH here that will be used by FindSDL2 module
set(SDL2_PATH "extern/SDL2")
find_package(SDL2 REQUIRED)

# Define SDL2_IMAGE_PATH here that will be used by FindSDL2_image module
set(SDL2_IMAGE_PATH "extern/SDL2_image")
find_package(SDL2_image REQUIRED)

# Build External Static Library
add_subdirectory(extern/glad)
add_subdirectory(extern/imgui)
add_subdirectory(extern/glm)
add_subdirectory(extern/stb)
add_subdirectory(extern/yaml-cpp)

# ===========================================================================================
# Build DYEngine library
add_library(DYEngine STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES})

# ===========================================================================================
# Precompiled Header
target_precompile_headers(DYEngine PRIVATE Precompiled.h)

# ===========================================================================================
# Include Headers to DYEngine
target_include_directories(DYEngine PUBLIC include)
# SDL2_INCLUDE_DIR variable set by the FindSDL2 module (if it finds directory).
target_include_directories(DYEngine PUBLIC ${SDL2_INCLUDE_DIR})     # This should be a private inclusion though, because of KeyCode.h implementation, this needs to be PUBLIC
target_include_directories(DYEngine PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})     # This should be a private inclusion though, because of KeyCode.h implementation, this needs to be PUBLIC
target_include_directories(DYEngine PRIVATE extern/glad/include)
target_include_directories(DYEngine PUBLIC extern/imgui)
target_include_directories(DYEngine PRIVATE extern/imgui/examples)
target_include_directories(DYEngine PUBLIC extern/glm)
target_include_directories(DYEngine PUBLIC extern/stb/include)
target_include_directories(DYEngine PUBLIC extern/yaml-cpp/include)

# ===========================================================================================
# Link External Libraries with DYEngine lib
target_link_libraries(DYEngine
        #${SDL2MAIN_LIBRARY}
        ${SDL2_LIBRARY} # SDL2MAIN_LIBRARY is also specified within SDL2_LIBRARY by the FindSDL2 module. It's needed for Windows specific main function. If I don't use FindSDL2 module, I have to link it as well.
        ${SDL2_IMAGE_LIBRARIES}
        glad
        imgui
        glm
        stb
        yaml-cpp)

# ===========================================================================================
# Copy default assets to the output directory
set(DEFAULT_ASSETS
        assets/Image.shader)

foreach(ASSET ${DEFAULT_ASSETS})
    # extract filename
    get_filename_component(ASSET_FILENAME ${ASSET} NAME)

    # set copy output directory
    if(WIN32)
        set(ASSET_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/default/${ASSET_FILENAME})
    else()
        set(ASSET_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/assets/default/${ASSET_FILENAME})
    endif(WIN32)

    message(STATUS "DYEngine Copy Default Asset: ${ASSET_OUTPUT_PATH}")
    configure_file(${ASSET} ${ASSET_OUTPUT_PATH})
endforeach()