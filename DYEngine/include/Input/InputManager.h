#pragma once

#include "Input/KeyCode.h"

#include <SDL.h>

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

	private:
		static std::unique_ptr<InputManager> s_Instance;

		// Normally we would use SDL_NUM_SCANCODES. But to save memory, we use 287 instead (SDL_SCANCODE_AUDIOFASTFORWARD + 1)
		static constexpr const int k_KeyArraySize = 287;

		std::array<bool, k_KeyArraySize> m_KeyboardKeys = {false};
		std::array<bool, k_KeyArraySize> m_PreviousKeyboardKeys = {false};
	};
}
