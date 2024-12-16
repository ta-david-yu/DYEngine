
# Copy all the text assets listed in ASSETS.
# The function would be executed whenever the text files have been modified.
function(CopyTextAssets ASSETS)
    # We merge the rest of the arguments with ASSETS to collect all the pass in assets as a list.
    # See - https://stackoverflow.com/questions/5248749/passing-a-list-to-a-cmake-macro
    set(ASSETS ${ASSETS} ${ARGN})

    foreach(ASSET ${ASSETS})
        # set input path (for debugging purpose)
        set(ASSET_INPUT_PATH ${PROJECT_SOURCE_DIR}/${ASSET})

        # set copy output path
        # for the difference between CMAKE_RUNTIME_OUTPUT_DIRECTORY and EXECUTABLE_OUTPUT_PATH, see - https://cmake.org/pipermail/cmake/2008-March/020310.html
        if(WIN32)
            set(ASSET_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ASSET})
        else()
            set(ASSET_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/${ASSET})
        endif(WIN32)

        message(STATUS "[${PROJECT_NAME}] copy text asset: '${ASSET_INPUT_PATH}' -> '${ASSET_OUTPUT_PATH}'")
        configure_file(${ASSET} ${ASSET_OUTPUT_PATH} COPYONLY)
    endforeach()
endfunction()

# Copy all the assets listed in ASSETS as if they are binary files (including text files).
# The function would only be executed if cmake configuration has been modified OR code has been modified and has to be rebuilt.
# Therefore if you want to update the assets manually, you should reload the cmake if none of the above is true.
function(CopyBinaryAssets ASSETS)
    # We merge the rest of the arguments with ASSETS to collect all the pass in assets as a list
    # for more details, see - https://stackoverflow.com/questions/5248749/passing-a-list-to-a-cmake-macro
    set(ASSETS ${ASSETS} ${ARGN})

    foreach(ASSET ${ASSETS})
        # set input path (for debugging purpose)
        set(ASSET_INPUT_PATH ${PROJECT_SOURCE_DIR}/${ASSET})

        # extract filepath
        get_filename_component(ASSET_PATH ${ASSET} DIRECTORY)
        get_filename_component(ASSET_NAME ${ASSET} NAME)

        # set copy output path
        # for the difference between CMAKE_RUNTIME_OUTPUT_DIRECTORY and EXECUTABLE_OUTPUT_PATH, see - https://cmake.org/pipermail/cmake/2008-March/020310.html
        if(WIN32)
            set(ASSET_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ASSET_PATH})
        else()
            set(ASSET_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/${ASSET_PATH})
        endif(WIN32)

        message(STATUS "[${PROJECT_NAME}] copy binary asset: '${ASSET_INPUT_PATH}' -> '${ASSET_OUTPUT_PATH}/${ASSET_NAME}'")
        file(COPY ${ASSET_INPUT_PATH} DESTINATION ${ASSET_OUTPUT_PATH})
    endforeach()
endfunction()

# Copy all the assets listed in ASSETS as if they are binary files (including text files) in post build stage of the specified target.
function(CopyAssetsPostBuild BUILD_TARGET ASSETS)
    # We merge the rest of the arguments with ASSETS to collect all the pass in assets as a list
    # for more details, see - https://stackoverflow.com/questions/5248749/passing-a-list-to-a-cmake-macro
    set(ASSETS ${ASSETS} ${ARGN})

    foreach(ASSET ${ASSETS})
        # set input path (for debugging purpose)
        set(ASSET_INPUT_PATH ${PROJECT_SOURCE_DIR}/${ASSET})

        # extract filepath
        get_filename_component(ASSET_PATH ${ASSET} DIRECTORY)
        get_filename_component(ASSET_NAME ${ASSET} NAME)

        add_custom_command(
                TARGET ${BUILD_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${ASSET_INPUT_PATH}
                $<TARGET_FILE_DIR:${BUILD_TARGET}>/${ASSET_PATH}/${ASSET_NAME}
                COMMENT "[${BUILD_TARGET}] Post Build Copy Binary Asset: '${ASSET_INPUT_PATH}' -> '$<TARGET_FILE_DIR:${BUILD_TARGET}>/${ASSET_PATH}'")
    endforeach()
endfunction()