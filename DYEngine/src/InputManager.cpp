#include "Input/InputManager.h"

#include "Util/Macro.h"

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
		for (int i = firstKeyScanIndex; i < k_KeyArraySize; i++)
		{
			m_KeyboardKeys[i] = states[i];
		}

		// Update mouse button states.
		std::uint32_t const buttonState = SDL_GetGlobalMouseState(&m_MouseX, &m_MouseY);
		for (int i = 0; i < k_MouseButtonCount; i++)
		{
			m_MouseButtons[i] = buttonState & SDL_BUTTON(i + 1);
		}
	}

	bool InputManager::GetKey(KeyCode keyCode) const
	{
		auto const scanCode = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keyCode));
		if (scanCode <= 0 || scanCode >= k_KeyArraySize)
		{
			return false;
		}

		return m_KeyboardKeys[scanCode];
	}

	bool InputManager::GetKeyDown(KeyCode keyCode) const
	{
		auto const scanCode = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keyCode));
		if (scanCode <= 0 || scanCode >= k_KeyArraySize)
		{
			return false;
		}

		return m_KeyboardKeys[scanCode] && !m_PreviousKeyboardKeys[scanCode];
	}

	bool InputManager::GetKeyUp(KeyCode keyCode) const
	{
		auto const scanCode = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keyCode));
		if (scanCode <= 0 || scanCode >= k_KeyArraySize)
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
}