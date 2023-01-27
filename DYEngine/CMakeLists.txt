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
        src/CameraProperties.cpp
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
        src/GamepadEvent.cpp src/Gamepad.cpp src/FPSCounter.cpp src/EasingFunctions.cpp)
set(HEADER_FILES
        include/Core/Application.h
        include/Graphics/WindowBase.h
        include/Graphics/SDLWindow.h
        include/Util/Time.h
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
        include/Scene/Entity.h
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
        include/Graphics/CameraProperties.h
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
        include/Input/Gamepad.h include/Util/FPSCounter.h include/Math/EasingFunctions.h)

message(STATUS "[DYEngine] Source Files: ${SOURCE_FILES}")
message(STATUS "[DYEngine] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External 3rd party library
# Define SDL2_PATH here that will be used by FindSDL2 module
set(SDL2_PATH "extern/SDL2")
find_package(SDL2 REQUIRED)

# Define SDL2_IMAGE_PATH here that will be used by FindSDL2_image module
set(SDL2_IMAGE_PATH "extern/SDL2_image")
find_package(SDL2_image REQUIRED)

# Define RTTR_DIR here that will be used in FindRTTR module
#set(RTTR_DIR "extern/rttr/rttr")
#find_package(RTTR CONFIG REQUIRED Core)

# Build External Static Library
add_subdirectory(extern/glad)
add_subdirectory(extern/imgui)
add_subdirectory(extern/glm)
add_subdirectory(extern/stb)
add_subdirectory(extern/yaml-cpp)
#add_subdirectory(extern/rttr)

# ===========================================================================================
# Build DYEngine library
add_library(DYEngine STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES})

# ===========================================================================================
# Include Headers to DYEngine
target_include_directories(DYEngine PUBLIC include)
# SDL2_INCLUDE_DIR variable set by the FindSDL2 module (if it finds directory).
target_include_directories(DYEngine PUBLIC ${SDL2_INCLUDE_DIR})     # This should be a private inclusion though, because of KeyCode.h implementation, this needs to be PUBLIC
target_include_directories(DYEngine PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})     # This should be a private inclusion though, because of KeyCode.h implementation, this needs to be PUBLIC
target_include_directories(DYEngine PRIVATE extern/glad/include)
target_include_directories(DYEngine PUBLIC extern/imgui)
target_include_directories(DYEngine PRIVATE extern/imgui/backends)
target_include_directories(DYEngine PUBLIC extern/glm)
target_include_directories(DYEngine PUBLIC extern/stb/include)
target_include_directories(DYEngine PUBLIC extern/yaml-cpp/include)
#target_include_directories(DYEngine PUBLIC extern/rttr/rttr/src)

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
        yaml-cpp
        #RTTR::Core_Lib
    )

# ===========================================================================================
# Copy default assets to the output directory
set(DEFAULT_ASSETS
        assets/DebugLineGizmo.shader
        assets/DebugGeometryGizmo.shader
        assets/SpriteDefault.shader
        assets/SpriteUnlit.shader)

foreach(ASSET ${DEFAULT_ASSETS})
    # extract filename
    get_filename_component(ASSET_FILENAME ${ASSET} NAME)

    # set copy output directory
    if(WIN32)
        set(ASSET_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/default/${ASSET_FILENAME})
    else()
        set(ASSET_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/assets/default/${ASSET_FILENAME})
    endif(WIN32)

    set(ASSET_INPUT_PATH ${PROJECT_SOURCE_DIR}/${ASSET})
    message(STATUS "[DYEngine] copy asset: '${ASSET_INPUT_PATH}' -> '${ASSET_OUTPUT_PATH}'")
    configure_file(${ASSET} ${ASSET_OUTPUT_PATH} COPYONLY)
endforeach()