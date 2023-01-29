cmake_policy(SET CMP0048 NEW)
project(DYEngineRoot)

cmake_minimum_required(VERSION 3.17)

# Cmake Flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CSS_STANDARD_LIBRARIES}")
#set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CSS_STANDARD_LIBRARIES}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Cmake Macros
include(${CMAKE_SOURCE_DIR}/cmake/Macros/RedirectOutputTo.cmake)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    RedirectOutputTo("out/Debug")
    add_compile_definitions(DYE_DEBUG)
    add_compile_definitions(DYE_OPENGL_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    RedirectOutputTo("out/Release")
endif ()

# Includes
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# DYEngine
add_subdirectory(DYEngine)

# Sandbox executable
add_subdirectory(Sandbox)

# MiniGamesDemo executable
add_subdirectory(MiniGamesDemo)

# TODO: Build DYEditor
