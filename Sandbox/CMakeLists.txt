project(Sandbox)

set(SOURCE_FILES
        src/SandboxApp.cpp
        src/SandboxLayer.cpp
        src/SandboxMessage.cpp
        src/StaticAABBColliderManager.cpp)

set(HEADER_FILES
        include/SandboxLayer.h
        include/SandboxMessage.h
        include/StaticAABBColliderManager.h
        include/TestComponents.h
        include/AnotherTestComponents.h)

set(GENERATED_FILES
        generated/UserTypeRegister.generated.cpp)

message(STATUS "[${PROJECT_NAME}] Header Files: ${HEADER_FILES}")
message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")

add_executable(Sandbox ${SOURCE_FILES} ${HEADER_FILES} ${GENERATED_FILES})

set(DEPEND_FILES
        ${HEADER_FILES})


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/UserTypeRegister.generated.cpp
    COMMAND DYEditorCodeGenerator
        ${CMAKE_CURRENT_SOURCE_DIR} # The source directory to put the generated code in
        ${HEADER_FILES}             # The list of headers to be parsed for generating code
    DEPENDS ${DEPEND_FILES}
    COMMENT "[${PROJECT_NAME}] Run DYEditor Code Generator"
)

target_include_directories(Sandbox PRIVATE include)
target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sandbox DYEngine)
target_link_libraries(Sandbox DYEntity)
target_link_libraries(Sandbox DYEditor)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(Sandbox -static-libgcc)
endif ()