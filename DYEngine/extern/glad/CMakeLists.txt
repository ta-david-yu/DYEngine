set(LIBRARY_NAME glad)

set(GLAD_SOURCE_FILES
        "src/glad.c")

include_directories("include")

add_library(${LIBRARY_NAME} STATIC
        ${GLAD_SOURCE_FILES})