set(LIBRARY_NAME glad)

set(GLAD_SOURCE_FILES
        "src/glad.c")

add_library(${LIBRARY_NAME} STATIC
        ${GLAD_SOURCE_FILES})

target_include_directories(${LIBRARY_NAME} PRIVATE include)