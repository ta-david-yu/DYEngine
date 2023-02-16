set(LIBRARY_NAME imgui)

# Imgui source files
set(IMGUI_SOURCE_FILES
        "imgui/imgui.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/imgui_widgets.cpp"
        "imgui/imgui_demo.cpp")

# Imgui source files needed for SDL2
set(IMGUI_SDL2_IMPL_SOURCE_FILES
        "imgui/backends/imgui_impl_sdl2.cpp"
        "imgui/backends/imgui_impl_opengl3.cpp")

add_library(${LIBRARY_NAME}
        ${IMGUI_SOURCE_FILES}
        ${IMGUI_SDL2_IMPL_SOURCE_FILES})

# SDL2_INCLUDE_DIR variable set by the FindSDL2 module (if it finds directory).
target_include_directories(${LIBRARY_NAME} PRIVATE ${SDL2_INCLUDE_DIR})
target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

target_link_libraries(${LIBRARY_NAME} ${SDL2_LIBRARY})
target_link_libraries(${LIBRARY_NAME} glad)

# Defining IMGUI_IMPL_OPENGL_LOADER_GLAD symbol will make imgui use glad as GL loader.
# Now it's commented out because imgui now has its own OpenGL loader (see backends/imgui_impl_opengl3_loader.h).
# For more details, see - https://github.com/ocornut/imgui/issues/4445.
#target_compile_definitions(${LIBRARY_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)