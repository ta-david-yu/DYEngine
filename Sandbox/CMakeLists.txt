project(Sandbox)

set(SOURCE_FILES
        src/SandboxApp.cpp
        src/SandboxLayer.cpp
        src/SandboxMessage.cpp
        src/StaticAABBColliderManager.cpp src/SystemExample.cpp)

set(HEADER_FILES
        include/SandboxLayer.h
        include/SandboxMessage.h
        include/StaticAABBColliderManager.h
        include/TestComponents.h
        include/AnotherTestComponents.h include/SystemExample.h)

set(INCLUDE_DIRS
        include
        ${CMAKE_CURRENT_SOURCE_DIR})

# Be sure to add "" around INCLUDE_DIRS, SOURCE_FILES and HEADER_FILES to make them arrays.
include(../DYEditor/cmake/Modules/DYEditorProject.cmake)
DYEditor_AddRuntimeExecutable(${PROJECT_NAME}_Runtime "${CMAKE_CURRENT_SOURCE_DIR}" "${INCLUDE_DIRS}" "${SOURCE_FILES}" "${HEADER_FILES}")
#DYEditor_AddEditorExecutable(${PROJECT_NAME}_Editor "${CMAKE_CURRENT_SOURCE_DIR}" "${INCLUDE_DIRS}" "${SOURCE_FILES}" "${HEADER_FILES}")

# Copy assets to the output directory
set(TEXT_ASSETS
        assets/Scenes/TestScene.tscene)

set(BINARY_ASSETS
        assets/Textures/DYE_logo_outlined.png
        assets/Textures/Sprite_DYEIcon.png)

include(../cmake/Modules/CopyAssets.cmake)
CopyTextAssets(${TEXT_ASSETS})
CopyBinaryAssets(${BINARY_ASSETS})