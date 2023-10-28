project(DYEditor)

option(DYE_BUILD_CODE_GENERATOR "Enable building DYEditor code generator tools" ON)

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
        src/SerializedComponent.cpp
        src/SerializedEntity.cpp
        src/SerializedScene.cpp
        src/SerializedSystemHandle.cpp
        src/Scene.cpp
        src/DYEditorApplication.cpp
        src/Entity.cpp
        src/World.cpp
        src/RuntimeState.cpp
        src/Render2DSpriteSystem.cpp
        src/RegisterCameraSystem.cpp
        src/EditorWindowManager.cpp
        src/RuntimeSceneManagement.cpp
        src/Undo.cpp
        src/EntityOperations.cpp
        src/SystemOperations.cpp
        src/UndoOperationBase.cpp
        src/HierarchyComponents.cpp
        src/EntityUtil.cpp
        src/SceneViewEntitySelection.cpp
        src/SerializedArray.cpp
        src/SubWindowConfiguration.cpp
        src/ExecuteLoadSceneCommandSystem.cpp
        src/AudioSystems.cpp src/WindowSystems.cpp
        src/TransformSystems.cpp
        src/StringUtil.cpp)
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
        include/Serialization/SerializedComponent.h
        include/Serialization/SerializedScene.h
        include/Serialization/SerializedSystemHandle.h
        include/DYEditorApplication.h
        include/Core/RuntimeState.h
        include/Core/Entity.h
        include/Core/World.h
        include/Core/WorldView.h
        include/Core/Components.h
        include/Components/SpriteRendererComponent.h
        include/Components/CameraComponent.h
        include/Components/NameComponent.h
        include/Components/TransformComponents.h
        include/Core/Systems.h
        include/Systems/Render2DSpriteSystem.h
        include/Systems/RegisterCameraSystem.h
        include/Configuration/ProjectConfig.h
        src/ProjectConfig.cpp
        include/ImGui/EditorWindowManager.h
        include/Core/RuntimeSceneManagement.h
        include/Components/IDComponent.h
        include/Undo/Undo.h
        include/Undo/UndoOperationBase.h
        include/Undo/Operations/EntityOperations.h
        include/Type/DrawComponentHeaderContext.h
        include/Undo/Operations/SystemOperations.h
        include/Components/HierarchyComponents.h
        include/Serialization/SerializedArray.h
        include/Util/EntityUtil.h
        include/SceneViewEntitySelection.h
        include/Configuration/SubWindowConfiguration.h
        include/Components/Command/LoadSceneComponent.h
        include/Systems/ExecuteLoadSceneCommandSystem.h
        include/Components/AudioSource2DComponent.h
        include/Components/Command/StartAudioSourceComponent.h
        include/Components/Command/StopAudioSourceComponent.h
        include/Systems/AudioSystems.h
        include/Components/PlayAudioSourceOnInitializeComponent.h
        include/Components/WindowComponents.h
        include/Systems/WindowSystems.h
        include/Components/Command/WindowCommandComponents.h
        include/Core/EntityTypes.h
        include/Systems/TransformSystems.h
        include/Util/StringUtil.h)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External library
add_subdirectory(extern/entt)
add_subdirectory(extern/tomlplusplus)

# ===========================================================================================
# Build library
add_library(DYEditor STATIC ${SOURCE_FILES} ${HEADER_FILES})

# ===========================================================================================
# Include headers
target_include_directories(DYEditor PUBLIC include)
target_include_directories(DYEditor PUBLIC ../DYEngine/include)
target_include_directories(DYEditor PUBLIC extern/entt/single_include)
target_include_directories(DYEditor PUBLIC extern/tomlplusplus/tomlplusplus/include)

# ===========================================================================================
# Link external libraries
target_link_libraries(DYEditor PUBLIC DYEngine)
target_link_libraries(DYEditor PRIVATE EnTT::EnTT)
target_link_libraries(DYEditor PRIVATE tomlplusplus::tomlplusplus)

# ===========================================================================================
if(DYE_BUILD_CODE_GENERATOR)
    add_subdirectory(DYEditorCodeGenerator)
endif()

# ===========================================================================================
# Copy default assets to the output directory
set(DEFAULT_ASSETS
        assets/default/EntityIDShader.shader)

include(../cmake/Modules/CopyAssets.cmake)
CopyTextAssets(${DEFAULT_ASSETS})
