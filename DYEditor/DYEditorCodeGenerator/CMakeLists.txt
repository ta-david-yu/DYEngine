project(DYEditorCodeGenerator)

# ===========================================================================================
# Sources / Headers
set(SOURCE_FILES
        src/main.cpp)
set(HEADER_FILES
        include/PropertyCodeGeneration.h include/ComponentCodeGeneration.h include/SystemCodeGeneration.h)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")

# ===========================================================================================
# External library
add_subdirectory(extern/fmt)

# ===========================================================================================
# Build executable
add_executable(DYEditorCodeGenerator ${SOURCE_FILES} ${HEADER_FILES})

# ===========================================================================================
# Include headers
target_include_directories(DYEditorCodeGenerator PRIVATE include)
#target_include_directories(DYEditorCodeGenerator PRIVATE ../include)
#target_include_directories(DYEditorCodeGenerator PRIVATE ../include)
#target_include_directories(DYEditorCodeGenerator PRIVATE ../../DYEngine/include)
#target_include_directories(DYEditorCodeGenerator PRIVATE ../../DYEntity/include)
target_include_directories(DYEditorCodeGenerator PRIVATE extern/fmt/include)

# ===========================================================================================
# Link external libraries
#target_link_libraries(DYEditorCodeGenerator PRIVATE DYEngine)
#target_link_libraries(DYEditorCodeGenerator PRIVATE DYEntity)
#target_link_libraries(DYEditorCodeGenerator PRIVATE DYEditor)
target_link_libraries(DYEditorCodeGenerator PRIVATE fmt::fmt-header-only)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(DYEditorCodeGenerator PRIVATE -static-libgcc)
endif ()