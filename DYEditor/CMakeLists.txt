project(DYEditor)

# ===========================================================================================
# Sources / Headers
set(SOURCE_FILES
        src/EntityLevelEditorLayer.cpp
        src/EntityLevelRuntimeLayer.cpp
        src/EntityLevelFile.cpp)
set(HEADER_FILES
        include/EntityLevelEditorLayer.h
        include/EntityLevelRuntimeLayer.h
        include/EntityLevelFile.h
        include/SerializableType.h)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External library

# ===========================================================================================
# Build library
add_library(DYEditor STATIC ${SOURCE_FILES} ${HEADER_FILES})

# ===========================================================================================
# Include headers
target_include_directories(DYEditor PUBLIC include)
target_include_directories(DYEditor PUBLIC ../DYEngine/include)
target_include_directories(DYEditor PUBLIC ../DYEntity/include)

# ===========================================================================================
# Link external libraries
target_link_libraries(DYEditor PUBLIC DYEngine)
target_link_libraries(DYEditor PUBLIC DYEntity)
