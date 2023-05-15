set(LIBRARY_NAME ImGuizmo)

set(IMGUIZMO_SOURCE_FILES
        "ImGuzmo/ImGuizmo.cpp")

message(STATUS "[${LIBRARY_NAME}]")

add_library(${LIBRARY_NAME} STATIC
        ${IMGUIZMO_SOURCE_FILES})

target_include_directories(${LIBRARY_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ImGuzmo)
