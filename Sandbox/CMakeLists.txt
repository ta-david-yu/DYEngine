project(Sandbox)

add_executable(Sandbox SandboxApp.cpp SandboxLayer.cpp)

target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Sandbox DYEngine)