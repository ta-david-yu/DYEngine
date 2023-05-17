# Having an extra folder level so we could hold all the settings call here instead of flooding the main engine CMakeLists.
set(LIBRARY_NAME FAudio)

# We set this because FAudio need SDL2_INCLUDE_DIRS to identify SDL2 include directory.
set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

# We set this because FAudio need SDL2_LIBRARIES to link SDL2 library.
set(SDL2_LIBRARIES ${SDL2_LIBRARY})

message(STATUS "[${LIBRARY_NAME}]")

add_subdirectory(FAudio)

