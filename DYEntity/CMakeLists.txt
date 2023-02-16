project(DYEntity)

# ===========================================================================================
# DYEngine Sources / Headers
set(SOURCE_FILES src/Entity.cpp src/World.cpp) ## TODO
set(HEADER_FILES include/Entity.h include/World.h) ## TODO

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External 3rd party library
add_subdirectory(extern/entt)

# ===========================================================================================
# Build DYEntity library
add_library(DYEntity STATIC ${SOURCE_FILES} ${HEADER_FILES})

# ===========================================================================================
# Include Headers to DYEntity
target_include_directories(DYEntity PUBLIC include)
target_include_directories(DYEntity PUBLIC ../DYEngine/include)
target_include_directories(DYEntity PRIVATE extern/entt/single_include)

# ===========================================================================================
# Link External Libraries with DYEntity lib
target_link_libraries(DYEntity INTERFACE DYEngine)
target_link_libraries(DYEntity PRIVATE EnTT::EnTT)