#pragma once

#include <cstdint>

#include <SDL.h>

namespace DYE
{
    /// KeyCode enumeration, currently mapped to SDL_KeyCode (event.key.keysym.sym)
    /// To be added with more keys
    enum class KeyCode
    {
        Space = SDL_KeyCode::SDLK_SPACE,

        Right = SDL_KeyCode::SDLK_RIGHT,
        Left = SDL_KeyCode::SDLK_LEFT,
        Down = SDL_KeyCode::SDLK_DOWN,
        Up = SDL_KeyCode::SDLK_UP,

        F1 = SDL_KeyCode::SDLK_F1,
        F2 = SDL_KeyCode::SDLK_F2,
        F3 = SDL_KeyCode::SDLK_F3
    };
}