cmake_policy(SET CMP0048 NEW)
project(DYEngineRoot)

cmake_minimum_required(VERSION 3.17)

# Cmake Flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cmake Macros
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
include(cmake/Modules/RedirectOutputTo.cmake)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    RedirectOutputTo("build/Debug")
    add_compile_definitions(DYE_DEBUG)
    add_compile_definitions(DYE_OPENGL_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    RedirectOutputTo("build/MinSizeRel")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    RedirectOutputTo("build/Release")
elseif (CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    RedirectOutputTo("build/MinSizeRel")
endif ()

# DYEngine library
add_subdirectory(DYEngine)

# DYEntity library
add_subdirectory(DYEntity)

# DYEditor library
add_subdirectory(DYEditor)

# Sandbox executable
add_subdirectory(Sandbox)
