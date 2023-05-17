set(LIBRARY_NAME raudio)

set(RAUDIO_HEADER_FILES
        "Macro.h"
        "raudio/src/raudio.h")

set(RAUDIO_SOURCE_FILES
        "raudio/src/raudio.c")

message(STATUS "[${LIBRARY_NAME}]")

add_library(${LIBRARY_NAME} STATIC
        ${RAUDIO_HEADER_FILES}
        ${RAUDIO_SOURCE_FILES})

# Compile raudio as a standalone library instead of a module of raylib (see raudio.c for further details).
target_compile_definitions(${LIBRARY_NAME} PRIVATE RAUDIO_STANDALONE)
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_MODULE_RAUDIO)

# Enable support for audio file formats (see raudio.c for further details).
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_FILEFORMAT_WAV)
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_FILEFORMAT_OGG)
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_FILEFORMAT_XM)
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_FILEFORMAT_MOD)
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_FILEFORMAT_FLAC)
target_compile_definitions(${LIBRARY_NAME} PRIVATE SUPPORT_FILEFORMAT_MP3)

# Include stb_vorbis implementation (see stb_vorbis.h for further details).
target_compile_definitions(${LIBRARY_NAME} PRIVATE STB_VORBIS_IMPLEMENTATION)

target_include_directories(${LIBRARY_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${LIBRARY_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/external)