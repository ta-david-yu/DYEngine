
# Generate DYEditor runtime type information code from a list of header files.
# The command in the function would be executed whenever any of the given headers is modified.
# If you want to force generate code every single build, use ForceGenerateDYEditorRTTICode instead.
function(GenerateDYEditorRTTICode SOURCE_DIR HEADER_FILES)
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
function(ForceGenerateDYEditorRTTICode SOURCE_DIR HEADER_FILES)
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
            DEPENDS AlwaysRebuild   # The command depends on a dummy target so it is executed every build
            COMMENT "[${PROJECT_NAME}] Force Run DYEditor Code Generator"
    )

    # Dummy output which is never actually produced. Anything that depends on
    # this will always be rebuilt.
    add_custom_command(
            OUTPUT AlwaysRebuild
            COMMAND cmake -E echo
    )
endfunction()