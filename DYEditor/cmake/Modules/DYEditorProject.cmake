
# There are 2 functions for building executable - 'DYEditor_AddEditorExecutable' and 'DYEditor_AddRuntimeExecutable'

# For both functions, remember to pass in INCLUDE_DIRS, SOURCE_FILES and HEADER_FILES parameters with "" enclosing them.
# For instance: DYEditor_AddEditorExecutable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR} "${INCLUDE_DIRS}" "${SOURCE_FILES}" "${HEADER_FILES}").
# So they are treated as respective parameters.

# Here are some related function options:
# DYE_FORCE_GENERATE_CODE - If enabled, the build function will always re-generate code for DYEditor::TypeRegistry even if the user code is unchanged.

# Build editor executable target called <NAME> from the given sources and generated code.
function(DYEditor_AddEditorExecutable NAME SOURCE_ROOT_DIR INCLUDE_DIRS SOURCE_FILES HEADER_FILES)

    message(STATUS "Build DYEditor Editor Executable '${NAME}'")

    DYEditor_AddExecutable(${NAME} ${SOURCE_ROOT_DIR} "${INCLUDE_DIRS}" "${SOURCE_FILES}" "${HEADER_FILES}")

    target_compile_definitions(${NAME} PUBLIC DYE_EDITOR)
    target_compile_definitions(DYEditor PUBLIC DYE_EDITOR)

    # Remove DYE_RUNTIME from DYEditor library explicitly because
    # users might also call DYEditor_AddRuntimeExecutable in the same CMakeLists.txt.
    # see - https://stackoverflow.com/questions/49049463/how-to-remove-definitions-per-target-in-cmake-2-8-12
    get_target_property(definitions DYEditor COMPILE_DEFINITIONS)
    list(FILTER definitions EXCLUDE REGEX [[^DYE_RUNTIME$]])
    set_property(TARGET DYEditor PROPERTY COMPILE_DEFINITIONS ${definitions})

endfunction()

# Build runtime executable target called <NAME> from the given sources and generated code.
function(DYEditor_AddRuntimeExecutable NAME SOURCE_ROOT_DIR INCLUDE_DIRS SOURCE_FILES HEADER_FILES)

    message(STATUS "Build DYEditor Runtime Executable '${NAME}'")

    DYEditor_AddExecutable(${NAME} ${SOURCE_ROOT_DIR} "${INCLUDE_DIRS}" "${SOURCE_FILES}" "${HEADER_FILES}")

    target_compile_definitions(${NAME} PUBLIC DYE_RUNTIME)
    target_compile_definitions(DYEditor PUBLIC DYE_RUNTIME)

    # Remove DYE_EDITOR from DYEditor library explicitly because
    # users might also call DYEditor_AddEditorExecutable in the same CMakeLists.txt.
    # see - https://stackoverflow.com/questions/49049463/how-to-remove-definitions-per-target-in-cmake-2-8-12
    get_target_property(definitions DYEditor COMPILE_DEFINITIONS)
    list(FILTER definitions EXCLUDE REGEX [[^DYE_EDITOR$]])
    set_property(TARGET DYEditor PROPERTY COMPILE_DEFINITIONS ${definitions})

endfunction()

function(DYEditor_AddExecutable NAME SOURCE_ROOT_DIR INCLUDE_DIRS SOURCE_FILES HEADER_FILES)

    set(DYE_PROJECT_NAME ${NAME})
    set(DYE_PROJECT_SOURCE_FILES ${SOURCE_FILES}) # Include generated code in the source as well
    set(DYE_PROJECT_HEADER_FILES ${HEADER_FILES})

    if (DYE_PROJECT_HEADER_FILES)
        # Include generated file if header files list is not empty.
        list(APPEND DYE_PROJECT_SOURCE_FILES generated/UserTypeRegister.generated.cpp)
    endif ()

    message(STATUS "[${DYE_PROJECT_NAME}] Source Files: ${DYE_PROJECT_SOURCE_FILES}")
    message(STATUS "[${DYE_PROJECT_NAME}] Header Files: ${DYE_PROJECT_HEADER_FILES}")

    # For now we always show the console, if you want to hide the console,
    # 1. Add WIN32 in the build command. Like this:
    #   add_executable(${DYE_PROJECT_NAME} WIN32 ${DYE_PROJECT_SOURCE_FILES} ${DYE_PROJECT_HEADER_FILES})
    # 2. Remove -mconsole when linking SDL2 in DYEngine.
    add_executable(${DYE_PROJECT_NAME} ${DYE_PROJECT_SOURCE_FILES} ${DYE_PROJECT_HEADER_FILES})

    target_link_libraries(${DYE_PROJECT_NAME} PRIVATE DYEngine)
    target_link_libraries(${DYE_PROJECT_NAME} PRIVATE DYEditor)

    target_include_directories(${DYE_PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

    if (DYE_PROJECT_HEADER_FILES)
        # Generate code if header files list is not empty.
        option(DYE_FORCE_GENERATE_CODE "If enabled, always re-generate code for DYEditor::TypeRegistry even if the code is unchanged." OFF)
        if (DYE_FORCE_GENERATE_CODE)
            ForceGenerateDYEditorTypeRegistryCode(${SOURCE_ROOT_DIR} ${DYE_PROJECT_HEADER_FILES})
        else ()
            GenerateDYEditorTypeRegistryCode(${SOURCE_ROOT_DIR} ${DYE_PROJECT_HEADER_FILES})
        endif ()
    endif ()

endfunction()

# Generate DYEditor runtime type information code from a list of header files.
# The command in the function would be executed whenever any of the given headers is modified.
# If you want to force generate code every single build, use ForceGenerateDYEditorRTTICode instead.
function(GenerateDYEditorTypeRegistryCode SOURCE_DIR HEADER_FILES)
    # We merge the rest of the arguments with HEADER_FILES to collect all the passed-in header files as a list.
    # See - https://stackoverflow.com/questions/5248749/passing-a-list-to-a-cmake-macro
    set(HEADER_FILES ${HEADER_FILES} ${ARGN})

    # This command essentially runs DYEditorCodeGenerator.exe
    # with arguments of the given source directory & the list of header files to be parsed.
    add_custom_command(
            OUTPUT ${SOURCE_DIR}/generated/UserTypeRegister.generated.cpp
            COMMAND DYEditorCodeGenerator
            ${SOURCE_DIR}               # The source directory to read headers from & to put the generated code in
            ${HEADER_FILES}             # The list of relative path to headers to be parsed for code generation
            DEPENDS ${HEADER_FILES}     # The command depends on header files so that whenever the headers files changed, the command is executed
            COMMENT "[${PROJECT_NAME}] Run DYEditor Code Generator"
    )
endfunction()

# Force generate DYEditor runtime type information code from a list of header files on every build.
function(ForceGenerateDYEditorTypeRegistryCode SOURCE_DIR HEADER_FILES)
    # We merge the rest of the arguments with HEADER_FILES to collect all the passed-in header files as a list.
    # See - https://stackoverflow.com/questions/5248749/passing-a-list-to-a-cmake-macro
    set(HEADER_FILES ${HEADER_FILES} ${ARGN})

    # see - https://stackoverflow.com/questions/13920072/how-to-always-run-command-when-building-regardless-of-any-dependency
    # For more details on executing command on every build
    add_custom_target(DummyTarget
            ALL
            DEPENDS ${SOURCE_DIR}/generated/UserTypeRegister.generated.cpp)

    # This command essentially runs DYEditorCodeGenerator.exe
    # with arguments of the given source directory & the list of header files to be parsed.
    add_custom_command(
            OUTPUT ${SOURCE_DIR}/generated/UserTypeRegister.generated.cpp
            COMMAND DYEditorCodeGenerator
            ${SOURCE_DIR}           # The source directory to read headers from & to put the generated code in
            ${HEADER_FILES}         # The list of relative path to headers to be parsed for code generation
            DEPENDS AlwaysRebuildCodeGenerator   # The command depends on a dummy target so it is executed every build
            COMMENT "[${PROJECT_NAME}] Force Run DYEditor Code Generator"
    )

    # Dummy output which is never actually produced. Anything that depends on
    # this will always be rebuilt.
    add_custom_command(
            OUTPUT AlwaysRebuildCodeGenerator
            COMMAND cmake -E echo
    )
endfunction()