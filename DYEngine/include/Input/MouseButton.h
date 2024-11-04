#pragma once

#include <SDL.h>

namespace DYE
{
    // SDL supports up to 5 mouse buttons - see SDL_BUTTON_LEFT.
    constexpr int const NumberOfMouseButtons = 5;
    enum class MouseButton
    {
        Left = SDL_BUTTON_LEFT,
        Middle = SDL_BUTTON_MIDDLE,
        Right = SDL_BUTTON_RIGHT,
        Extra1 = SDL_BUTTON_X1,
        Extra2 = SDL_BUTTON_X2
    };
}