#include "Input/InputManager.h"

#include "Util/Macro.h"
#include "ImGui/ImGuiUtil.h"

#include <imgui.h>

#include <algorithm>

namespace DYE
{
	std::unique_ptr<InputManager> InputManager::s_Instance = {};

	InputManager &InputManager::GetInstance()
	{
		DYE_ASSERT_RELEASE(
			s_Instance != nullptr &&
			"You might have forgot to call InputManager::InitSingleton beforehand?");
		return *s_Instance;
	}

	void InputManager::InitSingleton()
	{
		s_Instance = std::make_unique<InputManager>();
		s_Instance->ResetInputState();
	}

	void InputManager::HandleSystemEvent(Event &event)
	{
		auto const eventType = event.GetEventType();
		if (eventType == EventType::GamepadConnect)
		{
			auto const& controllerEvent = static_cast<const GamepadConnectEvent&>(event);
			handleOnGamepadConnected(controllerEvent);
		}
		else if (eventType == EventType::GamepadDisconnect)
		{
			auto const& controllerEvent = static_cast<const GamepadDisconnectEvent&>(event);
			handleOnGamepadDisconnected(controllerEvent);
		}
	}

	void InputManager::ResetInputState()
	{
		std::fill(m_KeyboardKeys.begin(), m_KeyboardKeys.end(), false);
		std::fill(m_PreviousKeyboardKeys.begin(), m_PreviousKeyboardKeys.end(), false);
	}

	void InputManager::UpdateInputState()
	{
		// Buffer the current states.

		std::copy(m_KeyboardKeys.begin(), m_KeyboardKeys.end(), m_PreviousKeyboardKeys.begin());
		std::copy(m_MouseButtons.begin(), m_MouseButtons.end(), m_PreviousMouseButtons.begin());
		m_PreviousMouseX = m_MouseX; m_PreviousMouseY = m_MouseY;

		// Update keyboard key states.
		const Uint8* states = SDL_GetKeyboardState(nullptr);
		auto const& firstKeyScanIndex = SDL_SCANCODE_A;
		for (int i = firstKeyScanIndex; i < NumberOfKeys; i++)
		{
			m_KeyboardKeys[i] = states[i];
		}

		// Update mouse button states.
		std::uint32_t const buttonState = SDL_GetGlobalMouseState(&m_MouseX, &m_MouseY);
		for (int i = 0; i < NumberOfMouseButtons; i++)
		{
			m_MouseButtons[i] = buttonState & SDL_BUTTON(i + 1);
		}
	}

	bool InputManager::GetKey(KeyCode keyCode) const
	{
		auto const scanCode = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keyCode));
		if (scanCode <= 0 || scanCode >= NumberOfKeys)
		{
			return false;
		}

		return m_KeyboardKeys[scanCode];
	}

	bool InputManager::GetKeyDown(KeyCode keyCode) const
	{
		auto const scanCode = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keyCode));
		if (scanCode <= 0 || scanCode >= NumberOfKeys)
		{
			return false;
		}

		return m_KeyboardKeys[scanCode] && !m_PreviousKeyboardKeys[scanCode];
	}

	bool InputManager::GetKeyUp(KeyCode keyCode) const
	{
		auto const scanCode = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keyCode));
		if (scanCode <= 0 || scanCode >= NumberOfKeys)
		{
			return false;
		}

		return !m_KeyboardKeys[scanCode] && m_PreviousKeyboardKeys[scanCode];
	}

	bool InputManager::GetMouseButton(MouseButton button) const
	{
		int const index = static_cast<int>(button) - 1;
		return m_MouseButtons[index];
	}

	bool InputManager::GetMouseButtonDown(MouseButton button) const
	{
		int const index = static_cast<int>(button) - 1;
		return m_MouseButtons[index] && !m_PreviousMouseButtons[index];
	}

	bool InputManager::GetMouseButtonUp(MouseButton button) const
	{
		int const index = static_cast<int>(button) - 1;
		return !m_MouseButtons[index] && m_PreviousMouseButtons[index];
	}

	std::optional<DeviceDescriptor> InputManager::GetDeviceDescriptor(DeviceID deviceId) const
	{
		if (deviceId >= m_DeviceDescriptors.size())
		{
			// Invalid device ID.
			return {};
		}

		return m_DeviceDescriptors[deviceId];
	}

	void InputManager::DrawDeviceDescriptorImGui() const
	{
		if (ImGui::Begin("InputManager - Device Descriptor"))
		{
			for (auto const& descriptor : m_DeviceDescriptors)
			{
				ImGuiUtil::DrawReadOnlyTextWithLabel(descriptor.Name, descriptor.ToString());
			}
		}

		ImGui::End();
	}

	void InputManager::handleOnGamepadConnected(const GamepadConnectEvent &connectEvent)
	{
		std::int32_t const deviceIndex = connectEvent.GetDeviceIndex();
		if (!SDL_GameControllerOpen(deviceIndex))
		{
			DYE_LOG_ERROR("InputManager::handleOnGamepadConnected(instanceId=%d) -> SDL_GameControllerOpen failed: %s", SDL_GetError());
			return;
		}

		DeviceInstanceID const instanceId = connectEvent.GetDeviceInstanceID();
		SDL_Joystick* sdlJoystick = SDL_JoystickFromInstanceID(instanceId);
		if (sdlJoystick == nullptr)
		{
			DYE_LOG_ERROR("InputManager::handleOnGamepadConnected(instanceId=%d) -> SDL_JoystickFromInstanceID failed: %s", instanceId, SDL_GetError());
			return;
		}

		// Get GUID.
		SDL_JoystickGUID const guid = SDL_JoystickGetGUID(sdlJoystick);
		char guidCStr[64] = {};
		SDL_JoystickGetGUIDString(guid, guidCStr, 64);
		std::string const guidString(guidCStr);

		// Generate or get Device ID.
		DeviceID deviceId = -1;
		auto deviceIDItr = m_DeviceIDTable.find(guidString);
		bool const hasDeviceID = deviceIDItr != m_DeviceIDTable.end();
		if (!hasDeviceID)
		{
			char const* cName = SDL_JoystickName(sdlJoystick);
			if (cName == nullptr)
			{
				DYE_LOG_ERROR("InputManager::handleOnGamepadConnected(%d) -> SDL_JoystickName failed: %s", deviceId, SDL_GetError());
				cName = "<no-name>";
			}

			// Create a device descriptor if this device has never been plugged before in the application session.
			deviceId = static_cast<DeviceID>(m_DeviceDescriptors.size());
			m_DeviceIDTable[guidString] = deviceId;
			m_DeviceDescriptors.emplace_back(DeviceDescriptor { .GUID = guidString, .Name = cName, .ID = deviceId, .InstanceID = instanceId });
		}
		else
		{
			// Acquire already-existing device deviceId and update instance deviceId.
			deviceId = m_DeviceIDTable[guidString];
			m_DeviceDescriptors[deviceId].InstanceID = instanceId;
		}

		m_NumberOfConnectedGamepads++;

		// TODO: initialize GamepadState etc.

		DYE_LOG("Gamepad Connected - %s, Gamepad Count = %d", m_DeviceDescriptors[deviceId].ToString().c_str(), m_NumberOfConnectedGamepads);
	}

	void InputManager::handleOnGamepadDisconnected(const GamepadDisconnectEvent &disconnectEvent)
	{
		DeviceInstanceID const instanceId = disconnectEvent.GetDeviceInstanceID();
		SDL_Joystick* sdlJoystick = SDL_JoystickFromInstanceID(instanceId);
		if (sdlJoystick == nullptr)
		{
			DYE_LOG_ERROR("InputManager::handleOnGamepadConnected(instanceId=%d) -> SDL_JoystickFromInstanceID failed: %s", instanceId, SDL_GetError());
			return;
		}

		// Get GUID.
		SDL_JoystickGUID const guid = SDL_JoystickGetGUID(sdlJoystick);
		char guidCStr[64] = {};
		SDL_JoystickGetGUIDString(guid, guidCStr, 64);
		std::string const guidString(guidCStr);

		// Generate or get Device ID.
		DeviceID deviceId = -1;
		if (!m_DeviceIDTable.contains(guidString))
		{
			// The disconnected device has never been registered in InputManager, ignore this disconnect event.
			return;
		}

		deviceId = m_DeviceIDTable[guidString];

		// Free SDL internal memory usage for the game controller object.
		SDL_GameController* sdlGameController = SDL_GameControllerFromInstanceID(instanceId);
		SDL_GameControllerClose(sdlGameController);

		// TODO: resize GamepadState etc.

		m_NumberOfConnectedGamepads--;
		DYE_LOG("Gamepad Disconnected - %s, Gamepad Count = %d", m_DeviceDescriptors[deviceId].ToString().c_str(), m_NumberOfConnectedGamepads);
	}
}