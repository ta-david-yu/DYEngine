project(DYEditor)

option(DYEDITOR_BUILD_CODE_GENERATOR "Enable building DYEditor code generator tools" ON)

# ===========================================================================================
# Sources / Headers
set(SOURCE_FILES
        src/SceneEditorLayer.cpp
        src/SceneRuntimeLayer.cpp
        src/TypeRegistry.cpp
        src/BuiltInTypeRegister.cpp
        src/UserTypeRegister.cpp
        src/EditorSystem.cpp
        src/EditorImGuiUtil.cpp
        src/SerializedObjectFactory.cpp
        src/SerializedComponentHandle.cpp
        src/SerializedEntity.cpp
        src/SerializedScene.cpp
        src/SerializedSystemHandle.cpp
        src/Scene.cpp
        src/DYEditorApplication.cpp
        src/RuntimeState.cpp
        src/Render2DSpriteSystem.cpp src/FileSystem.cpp src/RegisterCameraSystem.cpp src/EditorWindowManager.cpp)
set(HEADER_FILES
        include/SceneEditorLayer.h
        include/SceneRuntimeLayer.h
        include/Type/TypeRegistry.h
        include/Core/EditorCore.h
        include/Type/BuiltInTypeRegister.h
        include/Type/UserTypeRegister.h
        include/Type/DefaultComponentFunctions.h
        include/Core/EditorComponent.h
        include/Core/EditorSystem.h
        include/Core/EditorProperty.h
        include/Core/Scene.h
        include/ImGui/EditorImGuiUtil.h
        include/Serialization/SerializedObjectFactory.h
        include/Serialization/SerializedEntity.h
        include/Serialization/SerializedComponentHandle.h
        include/Serialization/SerializedScene.h
        include/Serialization/SerializedSystemHandle.h
        include/DYEditorApplication.h
        include/Core/RuntimeState.h
        include/Components.h
        include/Components/SpriteRendererComponent.h
        include/Systems.h
        include/Systems/Render2DSpriteSystem.h
        include/FileSystem/FileSystem.h include/Components/CameraComponent.h include/Systems/RegisterCameraSystem.h include/ProjectConfig.h src/ProjectConfig.cpp include/ImGui/EditorWindowManager.h)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External library
add_subdirectory(extern/tomlplusplus)

# ===========================================================================================
# Build library
add_library(DYEditor STATIC ${SOURCE_FILES} ${HEADER_FILES})

# ===========================================================================================
# Include headers
target_include_directories(DYEditor PUBLIC include)
target_include_directories(DYEditor PUBLIC ../DYEngine/include)
target_include_directories(DYEditor PUBLIC ../DYEntity/include)
target_include_directories(DYEditor PUBLIC extern/tomlplusplus/tomlplusplus/include)

# ===========================================================================================
# Link external libraries
target_link_libraries(DYEditor PUBLIC DYEngine)
target_link_libraries(DYEditor PUBLIC DYEntity)
target_link_libraries(DYEditor PRIVATE tomlplusplus::tomlplusplus)

# ===========================================================================================
if(DYEDITOR_BUILD_CODE_GENERATOR)
    add_subdirectory(DYEditorCodeGenerator)
endif()
