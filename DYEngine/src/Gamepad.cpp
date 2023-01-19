#include "Input/Gamepad.h"

#include <SDL.h>

namespace DYE
{
	std::string GetGamepadAxisName(GamepadAxis gamepadAxis)
	{
		return SDL_GameControllerGetStringForAxis(static_cast<SDL_GameControllerAxis>(gamepadAxis));
	}

	std::string GetGamepadButtonName(GamepadButton gamepadButton)
	{
		return SDL_GameControllerGetStringForButton(static_cast<SDL_GameControllerButton>(gamepadButton));
	}
}