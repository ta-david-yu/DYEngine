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
	private:
		struct GamepadState
		{
			DeviceID DeviceID;
			void* NativeGamepadObject = nullptr;

			std::array<bool, NumberOfGamepadButtons> Buttons = {false};
			std::array<bool, NumberOfGamepadButtons> PreviousButtons = {false};
			std::array<float, NumberOfGamepadAxes> Axes = {0.0f};
			std::array<float, NumberOfGamepadAxes> PreviousAxes = {0.0f};

			void Reset()
			{
				std::fill(Buttons.begin(), Buttons.end(), false);
				std::fill(PreviousButtons.begin(), PreviousButtons.end(), false);
				std::fill(Axes.begin(), Axes.end(), 0.0f);
				std::fill(PreviousAxes.begin(), PreviousAxes.end(), 0.0f);
			}
		};

	public:
		/// An application session could only have the number of devices with unique DeviceIDs.
		constexpr const static std::uint32_t MaxNumberOfUniqueGamepads = 32;

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

		glm::vec<2, std::int32_t> GetMousePositionRelativeToFocusedWindow() const;
		glm::vec<2, std::int32_t> GetMousePositionRelativeToWindow(std::uint32_t windowID) const;
		inline glm::vec<2, std::int32_t> GetGlobalMousePosition() const { return {m_MouseX, m_MouseY }; }
		inline glm::vec<2, std::int32_t> GetGlobalMouseDelta() const { return {m_MouseX - m_PreviousMouseX, m_MouseY - m_PreviousMouseY }; }

		bool GetMouseButton(MouseButton button) const;
		bool GetMouseButtonDown(MouseButton button) const;
		bool GetMouseButtonUp(MouseButton button) const;

		bool IsGamepadConnected(DeviceID deviceId) const;
		bool GetGamepadButton(DeviceID deviceId, GamepadButton button) const;
		bool GetGamepadButtonDown(DeviceID deviceId, GamepadButton button) const;
		bool GetGamepadButtonUp(DeviceID deviceId, GamepadButton button) const;
		float GetGamepadAxis(DeviceID deviceId, GamepadAxis axis) const;
		std::optional<DeviceDescriptor> GetGamepadDeviceDescriptor(DeviceID deviceId) const;

		void EnableGamepadInputEventInBackground();
		void DisableGamepadInputEventInBackground();

		void DrawInputManagerImGui(bool* pIsOpen = nullptr) const;

	private:
		void handleOnGamepadConnected(GamepadConnectEvent const& connectEvent);
		void handleOnGamepadDisconnected(GamepadDisconnectEvent const& disconnectEvent);

		/// Instead of returning a reference, we use pointer because we want to:
		/// 1. Indicate invalid parameter (i.e. device with the id not connected)
		/// 2. But still be able to reference the gamepad state without copying
		GamepadState const* getGamepadState(DeviceID deviceId) const;

		void drawDevicesInspectorImGui(std::vector<DeviceDescriptor> const& devices) const;

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

		std::unordered_map<std::string, DeviceID> m_RegisteredDeviceIDTable; 	// GUID string -> DeviceID
		std::vector<DeviceDescriptor> m_RegisteredDeviceDescriptors;

		std::int32_t m_NumberOfConnectedGamepads = 0;
		// Connected gamepad sparse set - dense array
		std::array<GamepadState, MaxNumberOfUniqueGamepads> m_GamepadStates;
		// Connected gamepad sparse set - sparse array
		// i.e. The index for device id 'x' in m_GamepadStates array is stored at m_GamepadStateIndices[x].
		std::array<std::int32_t, MaxNumberOfUniqueGamepads> m_GamepadStateIndices = {0};
	};
}
