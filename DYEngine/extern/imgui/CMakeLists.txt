set(LIBRARY_NAME imgui_sdl2)

# Imgui source files
set(IMGUI_SOURCE_FILES
        "imgui.cpp"
        "imgui_draw.cpp"
        "imgui_widgets.cpp"
        "imgui_demo.cpp")

# Imgui source files needed for SDL2
set(IMGUI_SDL2_IMPL_SOURCE_FILES
        "examples/imgui_impl_sdl.cpp"
        "examples/imgui_impl_opengl3.cpp")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("../glad/include")
include_directories("examples")

add_library(${LIBRARY_NAME}
        ${IMGUI_SOURCE_FILES}
        ${IMGUI_SDL2_IMPL_SOURCE_FILES})