#pragma once

#include <string>

#include <SDL.h>

namespace DYE
{
    /// KeyCode enumeration, currently mapped to SDL_KeyCode (event.key.keysym.sym)
    /// To be added with more keys
    enum class KeyCode
    {
		Escape = SDL_KeyCode::SDLK_ESCAPE,

        Space = SDL_KeyCode::SDLK_SPACE,

		W = SDL_KeyCode::SDLK_w,
		A = SDL_KeyCode::SDLK_a,
		S = SDL_KeyCode::SDLK_s,
		D = SDL_KeyCode::SDLK_d,

        Right = SDL_KeyCode::SDLK_RIGHT,
        Left = SDL_KeyCode::SDLK_LEFT,
        Down = SDL_KeyCode::SDLK_DOWN,
        Up = SDL_KeyCode::SDLK_UP,

        F1 = SDL_KeyCode::SDLK_F1,
        F2 = SDL_KeyCode::SDLK_F2,
        F3 = SDL_KeyCode::SDLK_F3,
		F4 = SDL_KeyCode::SDLK_F4,
		F5 = SDL_KeyCode::SDLK_F5,
		F6 = SDL_KeyCode::SDLK_F6,
		F7 = SDL_KeyCode::SDLK_F7,
		F8 = SDL_KeyCode::SDLK_F8
    };

	std::string GetKeyName(KeyCode keyCode);
}