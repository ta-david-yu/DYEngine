project(MiniGames)

add_executable(MiniGames
        MiniGamesApp.cpp
        ColliderManager.cpp
        CollisionTestLayer.cpp
        PongLayer.cpp
        Objects/PlayerPaddle.cpp
        Components/Transform.cpp Components/Sprite.cpp Components/Collider.cpp Objects/Wall.cpp Objects/PongBall.cpp Components/Velocity.cpp Objects/Camera.cpp Components/Hittable.cpp Objects/PongPlayer.cpp Objects/PongHomebase.cpp Components/AttachableToPaddle.cpp Objects/WindowCamera.cpp)

target_include_directories(MiniGames PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MiniGames DYEngine)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(MiniGames -static-libgcc)
endif ()

# Copy assets to the output directory
set(ASSETS
        assets/Sprite_Pong.png
        assets/Sprite_PongBorder.png
        assets/Sprite_DottedLine.png
        assets/Sprite_PongPaddle.png
        assets/Sprite_Grid.png
        assets/Sprite_GameOver.png
        assets/Sprite_P1Wins.png
        assets/Sprite_P2Wins.png)

# set copy output directory
if(WIN32)
    set(ASSET_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)
else()
    set(ASSET_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/assets/)
endif(WIN32)

foreach(ASSET ${ASSETS})
    # extract filename
    get_filename_component(ASSET_FILENAME ${ASSET} NAME)

    set(ASSET_INPUT_PATH ${PROJECT_SOURCE_DIR}/${ASSET})
    message(STATUS "[MiniGames] copy asset: '${ASSET_INPUT_PATH}' -> '${ASSET_OUTPUT_PATH}'")
    file(COPY ${ASSET_INPUT_PATH}
         DESTINATION ${ASSET_OUTPUT_PATH})
    # configure_file(${ASSET_INPUT_PATH} ${ASSET_OUTPUT_PATH} COPYONLY)
endforeach()