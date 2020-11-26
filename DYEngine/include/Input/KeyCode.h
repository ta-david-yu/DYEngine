#pragma once

#include <cstdint>

#include <SDL.h>

namespace DYE
{
    /// KeyCode enumeration, currently mapped to SDL_KeyCode (event.key.keysym.sym)
    /// To be added with more keys
    enum class KeyCode : std::uint32_t
    {
        Space = SDL_KeyCode::SDLK_SPACE,

        Right = SDL_KeyCode::SDLK_RIGHT,
        Left = SDL_KeyCode::SDLK_LEFT,
        Down = SDL_KeyCode::SDLK_DOWN,
        Up = SDL_KeyCode::SDLK_UP,
    };
}