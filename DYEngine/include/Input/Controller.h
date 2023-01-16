#pragma once

#include <string>

#include <SDL.h>

namespace DYE
{
	using DeviceIndex = std::int32_t;

	// Adapted from SDL - see SDL_GameControllerAxis.
	constexpr int const NumberOfControllerAxes = SDL_CONTROLLER_AXIS_MAX;
	enum class ControllerAxis
	{
		LeftStickHorizontal = SDL_CONTROLLER_AXIS_LEFTX,
		LeftStickVertical = SDL_CONTROLLER_AXIS_LEFTY,
		RightStickHorizontal = SDL_CONTROLLER_AXIS_RIGHTX,
		RightStickVertical = SDL_CONTROLLER_AXIS_RIGHTY,
		LeftTrigger = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
		RightTrigger = SDL_CONTROLLER_AXIS_TRIGGERRIGHT
	};

	// Adapted from SDL - see SDL_GameControllerButton.
	constexpr int const NumberOfControllerButtons = 15;
	enum class ControllerButton
	{
		A = SDL_CONTROLLER_BUTTON_A,
		B = SDL_CONTROLLER_BUTTON_B,
		X = SDL_CONTROLLER_BUTTON_X,
		Y = SDL_CONTROLLER_BUTTON_Y,
		Back = SDL_CONTROLLER_BUTTON_BACK,
		Guide = SDL_CONTROLLER_BUTTON_GUIDE,
		Start = SDL_CONTROLLER_BUTTON_START,
		LeftStick = SDL_CONTROLLER_BUTTON_LEFTSTICK,
		RightStick = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
		LeftShoulder = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
		RightShoulder = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
		DPadUp = SDL_CONTROLLER_BUTTON_DPAD_UP,
		DPadDown = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
		DPadLeft = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
		DPadRight = SDL_CONTROLLER_BUTTON_DPAD_RIGHT
	};
}