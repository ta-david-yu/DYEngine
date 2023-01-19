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

		for (int i = 0; i < m_NumberOfConnectedGamepads; i++)
		{
			m_GamepadStates[i].Reset();
		}
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
		if (deviceId >= m_RegisteredDeviceDescriptors.size())
		{
			// Invalid device ID.
			return {};
		}

		return m_RegisteredDeviceDescriptors[deviceId];
	}

	void InputManager::DrawAllRegisteredDeviceDescriptorsImGui() const
	{
		if (ImGui::Begin("InputManager - Registered Device Descriptor"))
		{
			for (auto const& descriptor : m_RegisteredDeviceDescriptors)
			{
				ImGuiUtil::DrawReadOnlyTextWithLabel(descriptor.Name, descriptor.ToString());
			}
		}

		ImGui::End();
	}

	void InputManager::DrawAllConnectedDeviceDescriptorsImGui() const
	{
		if (ImGui::Begin("InputManager - Connected Device Descriptor"))
		{
			for (int i = 0; i < m_NumberOfConnectedGamepads; i++)
			{
				GamepadState const& gamepadState = m_GamepadStates[i];
				DeviceDescriptor descriptor = GetDeviceDescriptor(gamepadState.DeviceID).value();
				ImGuiUtil::DrawReadOnlyTextWithLabel(descriptor.Name, descriptor.ToString());
			}
		}

		ImGui::End();
	}

	void InputManager::handleOnGamepadConnected(const GamepadConnectEvent &connectEvent)
	{
		std::int32_t const deviceIndex = connectEvent.GetDeviceIndex();
		DeviceInstanceID const instanceId = connectEvent.GetDeviceInstanceID();
		SDL_GameController* pSDLNativeGameController = SDL_GameControllerOpen(deviceIndex);
		if (pSDLNativeGameController == nullptr)
		{
			DYE_LOG_ERROR("InputManager::handleOnGamepadConnected(instanceId=%d) -> SDL_GameControllerOpen failed: %s", instanceId, SDL_GetError());
			return;
		}

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
		auto deviceIDItr = m_RegisteredDeviceIDTable.find(guidString);
		bool const hasDeviceID = deviceIDItr != m_RegisteredDeviceIDTable.end();
		if (!hasDeviceID)
		{
			if (m_RegisteredDeviceDescriptors.size() >= MaxNumberOfUniqueGamepads)
			{
				// It's a new device for this application session;
				// but the number of recorded devices has exceeded the max count.
				// Close the newly connected controller and ignore it.
				DYE_LOG("InputManager::handleOnGamepadConnected(instanceId=%d): "
							  "There have been %d gamepads connected, which is the max number of devices allowed per session."
							  "The newly connected device is ignored.", instanceId, MaxNumberOfUniqueGamepads);

				SDL_GameControllerClose(pSDLNativeGameController);
				return;
			}

			// Create a device descriptor if this device has never been plugged before in the application session.
			deviceId = static_cast<DeviceID>(m_RegisteredDeviceDescriptors.size());

			char const* cName = SDL_JoystickName(sdlJoystick);
			if (cName == nullptr)
			{
				DYE_LOG_ERROR("InputManager::handleOnGamepadConnected(%d) -> SDL_JoystickName failed: %s", deviceId, SDL_GetError());
				cName = "<no-name>";
			}

			m_RegisteredDeviceIDTable[guidString] = deviceId;
			m_RegisteredDeviceDescriptors.emplace_back(DeviceDescriptor { .GUID = guidString, .Name = cName, .ID = deviceId, .InstanceID = instanceId });
		}
		else
		{
			// Acquire already-existing device deviceId and update instance deviceId.
			deviceId = m_RegisteredDeviceIDTable[guidString];
			m_RegisteredDeviceDescriptors[deviceId].InstanceID = instanceId;
		}

		m_NumberOfConnectedGamepads++;
		m_GamepadStateIndices[deviceId] = m_NumberOfConnectedGamepads - 1;		// Make the index slot point to the last element in the dense array.

		GamepadState* pGamepadState = getGamepadState(deviceId);
		pGamepadState->DeviceID = deviceId;
		pGamepadState->NativeGamepadObject = pSDLNativeGameController;
		pGamepadState->Reset();

		DYE_LOG("Gamepad Connected - %s, Gamepad Count = %d", m_RegisteredDeviceDescriptors[deviceId].ToString().c_str(), m_NumberOfConnectedGamepads);
	}

	void InputManager::handleOnGamepadDisconnected(const GamepadDisconnectEvent &disconnectEvent)
	{
		DeviceInstanceID const instanceId = disconnectEvent.GetDeviceInstanceID();
		SDL_Joystick* sdlJoystick = SDL_JoystickFromInstanceID(instanceId);
		if (sdlJoystick == nullptr)
		{
			DYE_LOG_ERROR("InputManager::handleOnGamepadDisconnected(instanceId=%d) -> SDL_JoystickFromInstanceID failed: %s", instanceId, SDL_GetError());
			return;
		}

		// Get GUID.
		SDL_JoystickGUID const guid = SDL_JoystickGetGUID(sdlJoystick);
		char guidCStr[64] = {};
		SDL_JoystickGetGUIDString(guid, guidCStr, 64);
		std::string const guidString(guidCStr);

		// Generate or get Device ID.
		DeviceID deviceId = -1;
		if (!m_RegisteredDeviceIDTable.contains(guidString))
		{
			// The disconnected device has never been registered in InputManager, ignore this disconnect event.
			return;
		}

		deviceId = m_RegisteredDeviceIDTable[guidString];

		GamepadState* pGamepadState = getGamepadState(deviceId);
		if (pGamepadState == nullptr)
		{
			// This normally shouldn't happen:
			//
			// Null pointer means this device has not been properly
			// 1. Assigned a gamepad state slot
			// 2. Marked as connected
			// in the dense array for some reason (even though it has a registered device id).
			//
			DYE_LOG_ERROR("InputManager::handleOnGamepadDisconnected(deviceId=%d, instanceId=%d): "
						  "a registered device is connected although it was not marked as connected.", deviceId, instanceId);
			return;
		}

		// Free SDL internal memory usage for the game controller object.
		auto* sdlGameController = static_cast<SDL_GameController*>(pGamepadState->NativeGamepadObject);
		SDL_GameControllerClose(sdlGameController);

		// Swap the gamepad state slot with the last slot in the dense array.
		int const lastDenseArrayElementDeviceId = m_GamepadStates[m_NumberOfConnectedGamepads - 1].DeviceID;
		std::swap(m_GamepadStateIndices[deviceId], m_GamepadStateIndices[lastDenseArrayElementDeviceId]);
		std::swap(*pGamepadState, m_GamepadStates[m_NumberOfConnectedGamepads - 1]);

		m_NumberOfConnectedGamepads--;
		DYE_LOG("Gamepad Disconnected - %s, Gamepad Count = %d", m_RegisteredDeviceDescriptors[deviceId].ToString().c_str(), m_NumberOfConnectedGamepads);
	}

	InputManager::GamepadState* InputManager::getGamepadState(DeviceID deviceId)
	{
		int const denseArrayIndex = m_GamepadStateIndices[deviceId];
		bool const outOfValidBounds = denseArrayIndex >= m_NumberOfConnectedGamepads;
		if (outOfValidBounds)
		{
			// The index slot is pointing to an invalid element in the dense array,
			// which means the device is either disconnected or doesn't exist, return null.
			return nullptr;
		}

		bool const deviceIdMismatched = m_GamepadStates[denseArrayIndex].DeviceID != deviceId;
		if (deviceIdMismatched)
		{
			// The gamepad state stored at the location is not pointing back to device id,
			// which means the state has been taken by another device therefore the given device id is not connected, return null.
			return nullptr;
		}

		return &m_GamepadStates[denseArrayIndex];
	}
}