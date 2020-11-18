project(TestApp)

cmake_minimum_required(VERSION 3.17)

# Cmake Flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Cmake Macros
include(${CMAKE_SOURCE_DIR}/cmake/Macros/RedirectOutputTo.cmake)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    RedirectOutputTo("out/Debug")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    RedirectOutputTo("out/Release")
endif ()

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Sources
add_subdirectory(DYEngine)

add_executable(TestApp main.cpp)
target_link_libraries(TestApp DYEngine)