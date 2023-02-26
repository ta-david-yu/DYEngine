project(Sandbox)

add_executable(Sandbox SandboxApp.cpp SandboxLayer.cpp SandboxMessage.cpp StaticAABBColliderManager.cpp)

target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sandbox DYEngine)
target_link_libraries(Sandbox DYEntity)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(Sandbox -static-libgcc)
endif ()