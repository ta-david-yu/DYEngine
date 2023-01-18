#pragma once

#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Input/Gamepad.h"
#include "Event/GamepadEvent.h"
#include "Event/EventSystemBase.h"

#include <SDL.h>
#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <optional>

#define INPUT InputManager::GetInstance()

namespace DYE
{
	// Inherit from EventHandler to receive device connect/disconnect events from EventSystem.
	class InputManager
	{
	public:
		constexpr const static std::uint32_t MaxNumberOfConnectedGamepads = 32;

		static InputManager& GetInstance();

		/// Initialize internal input manager instance based on the platform.
		static void InitSingleton();

		InputManager() = default; // TODO: delete default constructor, because eventually the base class will become abstract/interface.
		InputManager(const InputManager &) = delete;
		virtual ~InputManager() = default;

		// Handle EventSystem events such as GamepadConnect.
		void HandleSystemEvent(Event &event);

		// Reset all the input states stored in the manager to false.
		void ResetInputState();

		// Force updating the input states stored in the manager.
		// Normally this function is called everytime when an input system event is fired therefore there is no need for
		// manual call in most cases.
		void UpdateInputState();

		bool GetKey(KeyCode keyCode) const;
		bool GetKeyDown(KeyCode keyCode) const;
		bool GetKeyUp(KeyCode keyCode) const;

		inline glm::vec<2, std::int32_t> GetGlobalMousePosition() const { return {m_MouseX, m_MouseY }; }
		inline glm::vec<2, std::int32_t> GetGlobalMouseDelta() const { return {m_MouseX - m_PreviousMouseX, m_MouseY - m_PreviousMouseY }; }

		bool GetMouseButton(MouseButton button) const;
		bool GetMouseButtonDown(MouseButton button) const;
		bool GetMouseButtonUp(MouseButton button) const;

		std::optional<DeviceDescriptor> GetDeviceDescriptor(DeviceID deviceId) const;
		std::optional<std::string> GetDeviceName(DeviceID deviceId) const;

		void DrawDeviceDescriptorImGui() const;

	private:
		void handleOnGamepadConnected(GamepadConnectEvent const& connectEvent);
		void handleOnGamepadDisconnected(GamepadDisconnectEvent const& disconnectEvent);

	private:
		static std::unique_ptr<InputManager> s_Instance;

		std::array<bool, NumberOfKeys> m_KeyboardKeys = {false};
		std::array<bool, NumberOfKeys> m_PreviousKeyboardKeys = {false};

		std::array<bool, NumberOfMouseButtons> m_MouseButtons = {false};
		std::array<bool, NumberOfMouseButtons> m_PreviousMouseButtons = {false};

		std::int32_t m_MouseX {0};
		std::int32_t m_MouseY {0};
		std::int32_t m_PreviousMouseX {0};
		std::int32_t m_PreviousMouseY {0};

		struct GamepadState
		{
			std::string GUID;
			void* NativeGamepadObject = nullptr;

			std::array<bool, NumberOfGamepadButtons> Buttons = {false};
			std::array<bool, NumberOfGamepadButtons> PreviousButtons = {false};
			std::array<float, NumberOfGamepadAxes> Axes = {false};
			std::array<float, NumberOfGamepadAxes> PreviousAxes = {false};
		};

		std::unordered_map<std::string, DeviceID> m_DeviceIDTable; 	// GUID string -> DeviceID
		std::vector<DeviceDescriptor> m_DeviceDescriptors;

		std::uint32_t m_NumberOfConnectedGamepads = 0;
		std::array<GamepadState, MaxNumberOfConnectedGamepads> m_GamepadStates;		// Connected gamepad sparse set - dense array
		std::array<int, MaxNumberOfConnectedGamepads> m_GamepadStateIndices = {0};	// Connected gamepad sparse set - sparse array
	};
}
