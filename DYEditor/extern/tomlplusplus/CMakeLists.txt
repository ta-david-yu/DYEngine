# Having an extra folder level so we could hold all the settings call here instead of flooding the main engine CMakeLists.
set(LIBRARY_NAME tomlplusplus)

message(STATUS "[${LIBRARY_NAME}]")

add_subdirectory(tomlplusplus)

# Putting compile definitions after add_subdirectory so we could target the built target.
target_compile_definitions(tomlplusplus_tomlplusplus INTERFACE TOML_EXCEPTIONS=0)

