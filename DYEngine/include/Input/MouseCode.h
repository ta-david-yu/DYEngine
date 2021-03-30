#pragma once

#include <cstdint>

#include <SDL.h>

namespace DYE
{
    enum class MouseCode
    {
        ButtonLeft      = SDL_BUTTON_LEFT,
        ButtonRight     = SDL_BUTTON_RIGHT,
        ButtonMiddle    = SDL_BUTTON_MIDDLE,

        ButtonX1        = SDL_BUTTON_X1,
        ButtonX2        = SDL_BUTTON_X2,
    };
}