set(LIBRARY_NAME glad)

set(GLAD_SOURCE_FILES
        "src/glad.c")

message(STATUS "[${LIBRARY_NAME}]")

add_library(${LIBRARY_NAME} STATIC
        ${GLAD_SOURCE_FILES})

target_include_directories(${LIBRARY_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)