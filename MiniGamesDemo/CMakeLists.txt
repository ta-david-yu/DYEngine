project(DYETechDemo)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

add_executable(DYETechDemo
        MiniGamesApp.cpp
        ColliderManager.cpp
        GizmosRippleEffectManager.cpp
        WindowParticlesManager.cpp
        Layers/CollisionTestLayer.cpp
        Layers/PongLayer.cpp
        Layers/MainMenuLayer.cpp
        Layers/LandTheBallLayer.cpp
        Components/Transform.cpp
        Components/Sprite.cpp
        Components/Collider.cpp
        Components/Velocity.cpp
        Components/Hittable.cpp
        Components/AttachableToPaddle.cpp
        Objects/PlayerPaddle.cpp
        Objects/Camera.cpp
        Objects/PongPlayer.cpp
        Objects/PongHomebase.cpp
        Objects/WindowCamera.cpp
        Objects/SpriteButton.cpp
        Objects/SpriteUnsignedNumber.cpp
        Objects/Wall.cpp
        Objects/PongBall.cpp
        Objects/LandBall.cpp
        ${APP_ICON_RESOURCE_WINDOWS})

target_include_directories(DYETechDemo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(DYETechDemo DYEngine)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    target_link_libraries(DYETechDemo -static-libgcc)
endif ()

# Copy assets to the output directory
set(BINARY_ASSETS
        assets/Sprite_Grid.png

        assets/Sprite_DYELogo.png

        assets/Sprite_RulePong.png
        assets/Sprite_RuleLandTheBall.png
        assets/Sprite_SubtitleExit.png

        assets/Sprite_ButtonLandTB_Selected.png
        assets/Sprite_ButtonLandTB_Deselected.png
        assets/Sprite_ButtonPong_Selected.png
        assets/Sprite_ButtonPong_Deselected.png
        assets/Sprite_ButtonExit_Selected.png
        assets/Sprite_ButtonExit_Deselected.png
        assets/Sprite_MenuButtonPrompt.png

        assets/Sprite_Digit_0.png
        assets/Sprite_Digit_1.png
        assets/Sprite_Digit_2.png
        assets/Sprite_Digit_3.png
        assets/Sprite_Digit_4.png
        assets/Sprite_Digit_5.png
        assets/Sprite_Digit_6.png
        assets/Sprite_Digit_7.png
        assets/Sprite_Digit_8.png
        assets/Sprite_Digit_9.png

        assets/Sprite_GameOver.png
        assets/Sprite_PressToDropBall.png
        assets/Sprite_PreviousHighScoreText.png
        assets/Sprite_NewHighScoreText.png
        assets/Sprite_HighScoreText.png

        assets/Sprite_Pong.png
        assets/Sprite_PongBorder.png
        assets/Sprite_DottedLine.png
        assets/Sprite_PongPaddle.png
        assets/Sprite_P1Wins.png
        assets/Sprite_P2Wins.png)

include(../DYEngine/cmake/Macros/CopyAssets.cmake)
CopyBinaryAssets(${BINARY_ASSETS})