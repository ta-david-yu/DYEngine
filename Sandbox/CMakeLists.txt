project(Sandbox)

set(SOURCE_FILES
        SandboxApp.cpp
        SandboxLayer.cpp
        SandboxMessage.cpp
        StaticAABBColliderManager.cpp)

set(HEADER_FILES
        include/TestComponentA.h)

# In the future this will be the whole codebase (Sandbox codebase)
set(DEPEND_FILES
        ${SOURCE_FILES}
        generated/_source_test/UserTypeRegister.template.cpp
        include/TestComponentA.h)

set(GENERATED_FILES
        generated/UserTypeRegister.generated.cpp)

message(STATUS "[${PROJECT_NAME}] Source Files: ${SOURCE_FILES}")
message(STATUS "[${PROJECT_NAME}] Generated Files: ${GENERATED_FILES}")

add_executable(Sandbox ${SOURCE_FILES} ${HEADER_FILES} ${GENERATED_FILES})
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/UserTypeRegister.generated.cpp
    COMMAND DYEditorCodeGenerator
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/_source_test/UserTypeRegister.template.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/UserTypeRegister.generated.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/TestComponentA.h
        ${SOURCE_FILES}
    DEPENDS ${DEPEND_FILES}
    COMMENT "[${PROJECT_NAME}] Run DYEditor Code Generator"
)

#add_custom_command(
#        TARGET Sandbox
#        PRE_BUILD
#        COMMAND DYEditorCodeGenerator arg1 arg2 arg3
#        COMMENT "[${PROJECT_NAME}] Run DYEditor Code Generator"
#)

target_include_directories(Sandbox PRIVATE include)
target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sandbox DYEngine)
target_link_libraries(Sandbox DYEntity)
target_link_libraries(Sandbox DYEditor)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(Sandbox -static-libgcc)
endif ()