project(DYEditor)

option(DYEDITOR_BUILD_CODE_GENERATOR "Enable building DYEditor code generator tools" ON)

# ===========================================================================================
# Sources / Headers
set(SOURCE_FILES
        src/EntitySceneEditorLayer.cpp
        src/EntitySceneRuntimeLayer.cpp
        src/TypeRegistry.cpp
        src/BuiltInTypeRegister.cpp
        src/UserTypeRegister.cpp
        src/EditorSystem.cpp
        src/EditorImGuiUtil.cpp
        src/SerializedObjectFactory.cpp src/SerializedComponentHandle.cpp src/SerializedEntity.cpp src/SerializedScene.cpp src/SerializedSystemHandle.cpp src/Scene.cpp)
set(HEADER_FILES
        include/EntitySceneEditorLayer.h
        include/EntitySceneRuntimeLayer.h
        include/TypeRegistry.h
        include/EditorCore.h
        include/BuiltInTypeRegister.h
        include/UserTypeRegister.h
        include/DefaultComponentFunctions.h
        include/EditorComponent.h
        include/EditorSystem.h
        include/EditorProperty.h
        include/ImGui/EditorImGuiUtil.h
        include/Serialization/SerializedObjectFactory.h
        include/Serialization/SerializedEntity.h
        include/Serialization/SerializedComponentHandle.h
        include/Serialization/SerializedScene.h include/Serialization/SerializedSystemHandle.h include/Scene.h)

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
