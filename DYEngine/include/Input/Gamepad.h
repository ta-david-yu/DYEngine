#pragma once

#include <string>
#include <sstream>

#include <SDL.h>

namespace DYE
{
	/// DeviceID is unique per joystick device in the same application session.
	/// i.e. a controller will still have the same DeviceID after re-plugging.
	/// You could use this to identify the same device in the same application session.
	using DeviceID = std::int32_t;

	/// DeviceInstanceID is unique per joystick plug-in session.
	/// i.e. everytime a controller is plugged, it is assigned with a new unique id that has never been used before.
	/// The implementation details depend on the underlying platform library.
	/// For instance in SDL, re-plugging a controller would change the instance id assigned to the controller.
	/// We recommend using DeviceID instead as the controller identifier.
	using DeviceInstanceID = std::int32_t;

	// Adapted from SDL - see SDL_GameControllerAxis.
	constexpr int const NumberOfGamepadAxes = SDL_CONTROLLER_AXIS_MAX;
	enum class GamepadAxis
	{
		LeftStickHorizontal = SDL_CONTROLLER_AXIS_LEFTX,
		LeftStickVertical = SDL_CONTROLLER_AXIS_LEFTY,
		RightStickHorizontal = SDL_CONTROLLER_AXIS_RIGHTX,
		RightStickVertical = SDL_CONTROLLER_AXIS_RIGHTY,
		LeftTrigger = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
		RightTrigger = SDL_CONTROLLER_AXIS_TRIGGERRIGHT
	};

	// Adapted from SDL - see SDL_GameControllerButton.
	constexpr int const NumberOfGamepadButtons = 15;
	enum class GamepadButton
	{
		A = SDL_CONTROLLER_BUTTON_A,
		B = SDL_CONTROLLER_BUTTON_B,
		X = SDL_CONTROLLER_BUTTON_X,
		Y = SDL_CONTROLLER_BUTTON_Y,

		South = SDL_CONTROLLER_BUTTON_A,
		East = SDL_CONTROLLER_BUTTON_B,
		West = SDL_CONTROLLER_BUTTON_X,
		North = SDL_CONTROLLER_BUTTON_Y,

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

	std::string GetGamepadAxisName(GamepadAxis gamepadAxis);
	std::string GetGamepadButtonName(GamepadButton gamepadButton);

	struct DeviceDescriptor
	{
		std::string GUID;
		std::string Name;
		DeviceID ID;

		DeviceInstanceID InstanceID;

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "Device: "<< "[GUID: " << GUID << ", ID: " << ID << ", InstanceID: " << InstanceID << "]";
			return ss.str();
		}
	};
}