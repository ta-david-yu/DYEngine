project(Pong)

add_executable(Pong PongApp.cpp PongLayer.cpp)

target_include_directories(Pong PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Pong DYEngine)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(Pong -static-libgcc)
endif ()