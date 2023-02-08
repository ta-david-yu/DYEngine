cmake_policy(SET CMP0048 NEW)
project(DYEngineRoot)

cmake_minimum_required(VERSION 3.17)

# Cmake Flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cmake Macros
include(cmake/Functions/RedirectOutputTo.cmake)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    RedirectOutputTo("out/Debug")
    add_compile_definitions(DYE_DEBUG)
    add_compile_definitions(DYE_OPENGL_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    RedirectOutputTo("out/Release")
endif ()

# DYEngine library
add_subdirectory(DYEngine)

# TODO: Build DYEditor

# Sandbox executable
add_subdirectory(Sandbox)
