project(DYEntity)

# ===========================================================================================
# Sources & headers
set(SOURCE_FILES
        src/Entity.cpp
        src/World.cpp)
set(HEADER_FILES
        include/Entity.h
        include/World.h
        include/NameComponent.h
        include/TransformComponent.h include/WorldView.h)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External library
add_subdirectory(extern/entt)

# ===========================================================================================
# Build library
add_library(DYEntity STATIC ${SOURCE_FILES} ${HEADER_FILES})

# ===========================================================================================
# Include headers
target_include_directories(DYEntity PUBLIC include)
target_include_directories(DYEntity PUBLIC ../DYEngine/include)
target_include_directories(DYEntity PUBLIC extern/entt/single_include)

# ===========================================================================================
# Link libraries
target_link_libraries(DYEntity PUBLIC DYEngine)
target_link_libraries(DYEntity PRIVATE EnTT::EnTT)