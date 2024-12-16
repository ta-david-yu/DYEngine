cmake_minimum_required(VERSION 3.17)
project(DYEngineRoot)

cmake_policy(SET CMP0048 NEW)

# Cmake Flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cmake Macros
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
include(cmake/Modules/RedirectOutputTo.cmake)

if (CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Multi-configuration generator detected: ${CMAKE_GENERATOR}")
    message(STATUS "Configure all configs output directories")
    foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
        RedirectOutputTo("build/${CONFIG_TYPE}")
    endforeach()
else ()
    message(STATUS "Single-configuration generator detected: ${CMAKE_GENERATOR}")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        RedirectOutputTo("build/Debug")
        add_compile_definitions(DYE_DEBUG)
        add_compile_definitions(DYE_OPENGL_DEBUG)
    elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        RedirectOutputTo("build/RelWithDebInfo")
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        RedirectOutputTo("build/Release")
    elseif (CMAKE_BUILD_TYPE MATCHES MinSizeRel)
        RedirectOutputTo("build/MinSizeRel")
    endif ()
endif ()

# DYEngine library
add_subdirectory(DYEngine)

# DYEditor library
add_subdirectory(DYEditor)

# Sandbox executable
add_subdirectory(Sandbox)
