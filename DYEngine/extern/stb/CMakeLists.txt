set(LIBRARY_NAME stb)

set(STB_SOURCE_FILES
        "src/stb_image.cpp")

add_library(${LIBRARY_NAME} STATIC
        ${STB_SOURCE_FILES})

target_include_directories(${LIBRARY_NAME} PUBLIC include)