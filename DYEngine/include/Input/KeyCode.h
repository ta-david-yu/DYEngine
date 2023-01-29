#pragma once

#include <string>

#include <SDL.h>

namespace DYE
{
	// Normally we would use SDL_NUM_SCANCODES. But to save memory, we use 287 instead - see SDL_SCANCODE_AUDIOFASTFORWARD + 1
	static constexpr const int NumberOfKeys = 287;

    /// KeyCode enumeration, currently mapped to SDL_KeyCode (event.key.keysym.sym)
    /// To be added with more keys
    enum class KeyCode
    {
		Escape = SDL_KeyCode::SDLK_ESCAPE,

        Space = SDL_KeyCode::SDLK_SPACE,
		Return = SDL_KeyCode::SDLK_RETURN,

		A = SDL_KeyCode::SDLK_a,
		D = SDL_KeyCode::SDLK_d,
		F = SDL_KeyCode::SDLK_f,
		G = SDL_KeyCode::SDLK_g,
		H = SDL_KeyCode::SDLK_h,
		S = SDL_KeyCode::SDLK_s,
		T = SDL_KeyCode::SDLK_t,
		W = SDL_KeyCode::SDLK_w,

        Right = SDL_KeyCode::SDLK_RIGHT,
        Left = SDL_KeyCode::SDLK_LEFT,
        Down = SDL_KeyCode::SDLK_DOWN,
        Up = SDL_KeyCode::SDLK_UP,

		Numpad1 = SDL_KeyCode::SDLK_KP_1,
		Numpad2 = SDL_KeyCode::SDLK_KP_2,
		Numpad3 = SDL_KeyCode::SDLK_KP_3,
		Numpad4 = SDL_KeyCode::SDLK_KP_4,
		Numpad5 = SDL_KeyCode::SDLK_KP_5,
		Numpad6 = SDL_KeyCode::SDLK_KP_6,
		Numpad7 = SDL_KeyCode::SDLK_KP_7,
		Numpad8 = SDL_KeyCode::SDLK_KP_8,
		Numpad9 = SDL_KeyCode::SDLK_KP_9,
		Numpad0 = SDL_KeyCode::SDLK_KP_0,

        F1 = SDL_KeyCode::SDLK_F1,
        F2 = SDL_KeyCode::SDLK_F2,
        F3 = SDL_KeyCode::SDLK_F3,
		F4 = SDL_KeyCode::SDLK_F4,
		F5 = SDL_KeyCode::SDLK_F5,
		F6 = SDL_KeyCode::SDLK_F6,
		F7 = SDL_KeyCode::SDLK_F7,
		F8 = SDL_KeyCode::SDLK_F8,
		F9 = SDL_KeyCode::SDLK_F9,
		F10 = SDL_KeyCode::SDLK_F10
    };

	std::string GetKeyName(KeyCode keyCode);
}