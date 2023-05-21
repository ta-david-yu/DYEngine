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
		B = SDL_KeyCode::SDLK_b,
		C = SDL_KeyCode::SDLK_c,
		D = SDL_KeyCode::SDLK_d,
		E = SDL_KeyCode::SDLK_e,
		F = SDL_KeyCode::SDLK_f,
		G = SDL_KeyCode::SDLK_g,
		H = SDL_KeyCode::SDLK_h,
		I = SDL_KeyCode::SDLK_i,
		J = SDL_KeyCode::SDLK_j,
		K = SDL_KeyCode::SDLK_k,
		L = SDL_KeyCode::SDLK_l,
		M = SDL_KeyCode::SDLK_m,
		N = SDL_KeyCode::SDLK_n,
		O = SDL_KeyCode::SDLK_o,
		P = SDL_KeyCode::SDLK_p,
		Q = SDL_KeyCode::SDLK_q,
		R = SDL_KeyCode::SDLK_r,
		S = SDL_KeyCode::SDLK_s,
		T = SDL_KeyCode::SDLK_t,
		W = SDL_KeyCode::SDLK_w,
		X = SDL_KeyCode::SDLK_x,
		Y = SDL_KeyCode::SDLK_y,
		Z = SDL_KeyCode::SDLK_z,

        F1 = SDL_KeyCode::SDLK_F1,
        F2 = SDL_KeyCode::SDLK_F2,
        F3 = SDL_KeyCode::SDLK_F3,
		F4 = SDL_KeyCode::SDLK_F4,
		F5 = SDL_KeyCode::SDLK_F5,
		F6 = SDL_KeyCode::SDLK_F6,
		F7 = SDL_KeyCode::SDLK_F7,
		F8 = SDL_KeyCode::SDLK_F8,
		F9 = SDL_KeyCode::SDLK_F9,
		F10 = SDL_KeyCode::SDLK_F10,

		Delete = SDL_KeyCode::SDLK_DELETE,

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

		Num1 = SDL_KeyCode::SDLK_1,
		Num2 = SDL_KeyCode::SDLK_2,
		Num3 = SDL_KeyCode::SDLK_3,
		Num4 = SDL_KeyCode::SDLK_4,
		Num5 = SDL_KeyCode::SDLK_5,
		Num6 = SDL_KeyCode::SDLK_6,
		Num7 = SDL_KeyCode::SDLK_7,
		Num8 = SDL_KeyCode::SDLK_8,
		Num9 = SDL_KeyCode::SDLK_9,
		Num0 = SDL_KeyCode::SDLK_0,

		NumpadPlus = SDL_KeyCode::SDLK_KP_PLUS,
		NumpadMinus = SDL_KeyCode::SDLK_KP_MINUS,

		LeftControl = SDL_KeyCode::SDLK_LCTRL,
		LeftShift = SDL_KeyCode::SDLK_LSHIFT,
		LeftAlt = SDL_KeyCode::SDLK_LALT,
		RightControl = SDL_KeyCode::SDLK_RCTRL,
		RightShift = SDL_KeyCode::SDLK_RSHIFT,
		RightAlt = SDL_KeyCode::SDLK_RALT,
    };

	std::string GetKeyName(KeyCode keyCode);
}