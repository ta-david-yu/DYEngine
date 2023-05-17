project(DYEngine VERSION 0.0.1)

# ===========================================================================================
# Sources & headers
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
        src/GameObject.cpp
        src/ComponentBase.cpp
        src/TypeUtil.cpp
        src/Transform.cpp
        src/UniformType.cpp
        src/Shader.cpp
        src/OpenGL.cpp
        src/Buffer.cpp
        src/VertexArray.cpp
        src/RenderCommand.cpp
        src/ImageRenderer.cpp
        src/Texture.cpp
        src/Camera.cpp
        src/ImGuiUtil.cpp
        src/ShaderProcessorBase.cpp
        src/Material.cpp
        src/RenderState.cpp
        src/UniformPropertyProcessor.cpp
        src/BlendStateCommandProcessor.cpp
        src/DepthStateCommandProcessor.cpp
        src/RenderPipelineManager.cpp
        src/RenderPipelineBase.cpp
        src/RenderPipeline2D.cpp
        src/KeyCode.cpp
        src/InputManager.cpp
        src/MaterialPropertyBlock.cpp
        src/WindowManager.cpp
        src/Screen.cpp
        src/AABB.cpp
        src/DebugDraw.cpp
        src/Math.cpp
        src/Color.cpp
        src/PrimitiveTest.cpp
        src/GamepadEvent.cpp
        src/Gamepad.cpp
        src/FPSCounter.cpp
        src/EasingFunctions.cpp
        src/Framebuffer.cpp
        src/FileSystem.cpp
        src/GUID.cpp src/AudioClip.cpp
        src/AudioSystem.cpp src/AudioSource.cpp)
set(HEADER_FILES
        include/Core/Application.h
        include/Graphics/WindowBase.h
        include/Graphics/SDLWindow.h
        include/Core/Time.h
        include/Event/Event.h
        include/Event/ApplicationEvent.h
        include/Input/KeyCode.h
        include/Event/KeyEvent.h
        include/Event/EventSystemBase.h
        include/Event/SDLEventSystem.h
        include/Util/Macro.h
        include/Graphics/ContextBase.h
        include/Graphics/SDLContext.h
        include/AppEntryPoint.h
        include/Core/LayerBase.h
        include/Core/LayerStack.h
        include/ImGui/ImGuiLayer.h
        include/Util/Logger.h
        include/Scene/SceneLayer.h
        include/Scene/GameObject.h
        include/Scene/ComponentBase.h
        include/Util/TypeUtil.h
        include/Scene/Transform.h
        include/Graphics/UniformType.h
        include/Graphics/Shader.h
        include/Graphics/OpenGL.h
        include/Graphics/Buffer.h
        include/Graphics/VertexArray.h
        include/Graphics/RenderCommand.h
        include/Scene/ImageRenderer.h
        include/Graphics/Texture.h
        include/Event/MouseEvent.h
        include/Message/Messaging.h
        include/Graphics/Camera.h
        include/ImGui/ImGuiUtil.h
        include/Graphics/ShaderProcessorBase.h
        include/Graphics/Material.h
        include/Graphics/RenderState.h
        include/Graphics/Processor/UniformPropertyProcessor.h
        include/Graphics/Processor/BlendStateCommandProcessor.h
        include/Graphics/Processor/DepthStateCommandProcessor.h
        include/Graphics/RenderPipelineManager.h
        include/Graphics/RenderPipelineBase.h
        include/Graphics/RenderPipeline2D.h
        include/Util/Algorithm.h
        include/Input/InputManager.h
        include/Graphics/MaterialPropertyBlock.h
        include/Graphics/WindowManager.h
        include/Math/Rect.h
        include/Math/Math.h
        include/Screen.h
        include/Math/AABB.h
        include/Math/PrimitiveTest.h
        include/Graphics/DebugDraw.h
        include/Math/Color.h
        include/Event/GamepadEvent.h
        include/Input/MouseButton.h
        include/Input/Gamepad.h
        include/Util/FPSCounter.h
        include/Math/EasingFunctions.h
        include/Graphics/Framebuffer.h
        include/FileSystem/FileSystem.h
        include/Core/GUID.h
        include/ImGui/ImGuiUtil_Internal.h
        include/Audio/AudioClip.h
        include/Audio/AudioSystem.h include/Audio/AudioSource.h)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# External library
# Define SDL2_PATH here that will be used by FindSDL2 module
if (MSVC)
    set(SDL2_PATH "extern/SDL2/MSVC")
elseif (MINGW)
    set(SDL2_PATH "extern/SDL2/MingGW")
endif ()
find_package(SDL2 REQUIRED)

# Build external static library
add_subdirectory(extern/glad)
add_subdirectory(extern/imgui)
add_subdirectory(extern/glm)
add_subdirectory(extern/stb)

add_subdirectory(extern/ImGuizmo)
target_include_directories(ImGuizmo PRIVATE extern/imgui/imgui) # ImGuizmo depends on imgui's

#add_subdirectory(extern/SDL_sound)
#add_subdirectory(extern/FAudio)
add_subdirectory(extern/raudio)

# ===========================================================================================
# Build library
add_library(DYEngine STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES})

# ===========================================================================================
# Include headers
target_include_directories(DYEngine PUBLIC include)
# SDL2_INCLUDE_DIR variable is set by the FindSDL2 module (if it finds the directory with SDL2).
# Ideally SDL2 should be included privately. But because of KeyCode.h/Logger.h implementation, it needs to be PUBLIC for now.
target_include_directories(DYEngine PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(DYEngine PUBLIC extern/glad/include)
target_include_directories(DYEngine PUBLIC extern/imgui/imgui)
target_include_directories(DYEngine PUBLIC extern/imgui/imgui/misc/cpp)
# imgui/backends is needed by ImGuiLayer implementation (ImGuiLayer.cpp)
target_include_directories(DYEngine PRIVATE extern/imgui/imgui/backends)
target_include_directories(DYEngine PUBLIC extern/glm)
target_include_directories(DYEngine PUBLIC extern/stb/include)
target_include_directories(DYEngine PUBLIC extern/ImGuizmo/ImGuzmo)
target_include_directories(DYEngine PUBLIC extern/raudio/raudio/src)

# ===========================================================================================
# Link external libraries

# SDL2MAIN_LIBRARY is also specified within SDL2_LIBRARY by the FindSDL2 module. It's needed for Windows specific main function. If I don't use FindSDL2 module, I have to link it as well.
#target_link_libraries(DYEngine ${SDL2MAIN_LIBRARY})
target_link_libraries(DYEngine PRIVATE ${SDL2_LIBRARY} -mconsole) # Add -mconsole so the console window is shown.
target_link_libraries(DYEngine PRIVATE glad)
target_link_libraries(DYEngine PRIVATE imgui)
target_link_libraries(DYEngine PRIVATE glm)
target_link_libraries(DYEngine PRIVATE stb)
target_link_libraries(DYEngine PRIVATE ImGuizmo)
target_link_libraries(DYEngine PRIVATE raudio)

# Link winpthread because SDL2 needs it.
# We need to specify -static because winpthread is included in the bundled MinGW distribution that comes with CLion.
# We would have to modify this for other compilers at some point.
if (MINGW)
    target_link_libraries(DYEngine PRIVATE -static winpthread)
endif ()

# ===========================================================================================
# Copy SDL2.dll to the output directory post-build
if (MSVC)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8) # 64 bit
        set(SDL2_DLL_PATH ${PROJECT_SOURCE_DIR}/${SDL2_PATH}/lib/x64/SDL2.dll)
    else () # 32 bit
        set(SDL2_DLL_PATH ${PROJECT_SOURCE_DIR}/${SDL2_PATH}/lib/x86/SDL2.dll)
    endif ()
elseif (MINGW)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8) # 64 bit
        set(SDL2_DLL_PATH ${PROJECT_SOURCE_DIR}/${SDL2_PATH}/bin/x64/SDL2.dll)
    else () # 32 bit
        set(SDL2_DLL_PATH ${PROJECT_SOURCE_DIR}/${SDL2_PATH}/bin/x86/SDL2.dll)
    endif ()
endif ()

add_custom_command(TARGET DYEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${SDL2_DLL_PATH}
        $<TARGET_FILE_DIR:DYEngine>/
        COMMENT "${PROJECT_NAME} Post Build Copy SDL2.dll")

# ===========================================================================================
# Copy default assets to the output directory
set(DEFAULT_ASSETS
        assets/default/DebugLineGizmo.shader
        assets/default/DebugGeometryGizmo.shader
        assets/default/SpriteDefault.shader
        assets/default/SpriteUnlit.shader)

include(../cmake/Modules/CopyAssets.cmake)
CopyTextAssets(${DEFAULT_ASSETS})