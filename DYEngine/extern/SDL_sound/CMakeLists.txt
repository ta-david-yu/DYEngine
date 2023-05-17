# Having an extra folder level so we could hold all the settings call here instead of flooding the main engine CMakeLists.
set(LIBRARY_NAME SDL_sound)

# We set this because SDL_sound need SDL2_INCLUDE_DIRS to identify SDL2 include directory
set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

message(STATUS "[${LIBRARY_NAME}]")

add_subdirectory(SDL_sound)

