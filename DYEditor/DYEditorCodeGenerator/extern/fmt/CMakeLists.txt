# Having an extra folder level so we could hold all the settings call here instead of flooding the main engine CMakeLists.
set(LIBRARY_NAME fmt)

add_subdirectory(fmt)

# Putting compile definitions after add_subdirectory so we could target the built target.
# Force make fmt library non-exceptions.
target_compile_definitions(fmt PUBLIC FMT_EXCEPTIONS=0)
target_compile_definitions(fmt-header-only INTERFACE FMT_EXCEPTIONS=0)

