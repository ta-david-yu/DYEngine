#pragma once

#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

#include <SDL.h>
#include <glm/glm.hpp>

#include <memory>
#include <array>

#define INPUT InputManager::GetInstance()

namespace DYE
{
	class InputManager
	{
	public:
		static InputManager& GetInstance();

		/// Initialize internal input manager instance based on the platform.
		static void InitSingleton();

		InputManager() = default; // TODO: delete default constructor, because eventually the base class will become abstract/interface.
		InputManager(const InputManager &) = delete;

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

	private:
		static std::unique_ptr<InputManager> s_Instance;

		// Keyboard:

		// Normally we would use SDL_NUM_SCANCODES. But to save memory, we use 287 instead (SDL_SCANCODE_AUDIOFASTFORWARD + 1)
		static constexpr const int k_KeyArraySize = 287;
		std::array<bool, k_KeyArraySize> m_KeyboardKeys = {false};
		std::array<bool, k_KeyArraySize> m_PreviousKeyboardKeys = {false};

		// Mouse:

		std::int32_t m_MouseX {0};
		std::int32_t m_MouseY {0};
		std::int32_t m_PreviousMouseX {0};
		std::int32_t m_PreviousMouseY {0};

		// SDL supports up to 5 mouse buttons (SDL_BUTTON_LEFT)
		static constexpr const int k_MouseButtonCount = 5;
		std::array<bool, k_MouseButtonCount> m_MouseButtons = {false};
		std::array<bool, k_MouseButtonCount> m_PreviousMouseButtons = {false};
	};
}
