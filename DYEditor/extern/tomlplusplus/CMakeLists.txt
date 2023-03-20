set(LIBRARY_NAME tomlplusplus)

# To disable exception error handling
add_compile_definitions(TOML_EXCEPTIONS=0)

add_subdirectory(tomlplusplus)

# To disable exception error handling
target_compile_definitions(tomlplusplus_tomlplusplus INTERFACE TOML_EXCEPTIONS=0)

