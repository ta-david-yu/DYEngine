set(LIBRARY_NAME imgui_sdl2)

set(SRC_GRP_IMGUI
        imgui.cpp
        imgui_draw.cpp
        imgui_widgets.cpp
        imgui_demo.cpp
        )

set(SRC_GRP_IMGUI_SDL2_IMPL
        examples/imgui_impl_sdl.cpp
        examples/imgui_impl_opengl3.cpp
        )

set(SRC_GRP_IMGUI_GL3W
        examples/libs/gl3w/GL/gl3w.c
        )

set(LIBRARY_SRCS
        ${SRC_GRP_IMGUI}
        ${SRC_GRP_IMGUI_SDL2_IMPL}
        ${SRC_GRP_IMGUI_GL3W}
        )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples
)

include_directories(SYSTEM
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/libs/gl3w
        ${SDL2_INCLUDE_DIR}
        )

add_library(${LIBRARY_NAME} STATIC
        ${LIBRARY_SRCS}
        )

target_link_libraries(
        ${LIBRARY_NAME}
        ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${CMAKE_DL_LIBS}
)

## Imgui source files
#set(IMGUI_SOURCE_FILES
#        "extern/imgui/imgui.cpp"
#        "extern/imgui/imgui_draw.cpp"
#        "extern/imgui/imgui_widgets.cpp"
#        "extern/imgui/imgui_demo.cpp")
#
## Imgui source files needed for SDL2
#set(IMGUI_SDL2_IMPL_SOURCE_FILES
#        "extern/imgui/examples/imgui_impl_sdl.cpp"
#        "extern/imgui/examples/imgui_impl_opengl3.cpp")


#target_include_directories(DYEngine INTERFACE extern/imgui)
#target_compile_definitions(DYEngine INTERFACE IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL_opengl.h> GL_GLEXT_PROTOTYPES=1)
