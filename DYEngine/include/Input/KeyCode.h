#pragma once

#include <string>
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

		W = SDL_KeyCode::SDLK_w,
		A = SDL_KeyCode::SDLK_a,
		S = SDL_KeyCode::SDLK_s,
		D = SDL_KeyCode::SDLK_d,

        F1 = SDL_KeyCode::SDLK_F1,
        F2 = SDL_KeyCode::SDLK_F2,
        F3 = SDL_KeyCode::SDLK_F3
    };

	std::string GetKeyName(KeyCode keyCode);
}