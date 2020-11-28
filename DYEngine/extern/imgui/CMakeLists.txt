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

add_library(${LIBRARY_NAME}
        ${IMGUI_SOURCE_FILES}
        ${IMGUI_SDL2_IMPL_SOURCE_FILES})

# SDL2_INCLUDE_DIR variable set by the FindSDL2 module (if it finds directory).
target_include_directories(${LIBRARY_NAME} PRIVATE ${SDL2_INCLUDE_DIR})
target_include_directories(${LIBRARY_NAME} PRIVATE "../glad/include")
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${LIBRARY_NAME} INTERFACE "examples")

target_link_libraries(${LIBRARY_NAME}
        ${SDL2_LIBRARY})