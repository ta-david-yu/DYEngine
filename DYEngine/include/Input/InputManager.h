#pragma once

#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Input/Controller.h"

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

		struct ControllerState
		{
			DeviceIndex DeviceIndex = -1;
			void* NativeControllerData = nullptr;

			std::array<bool, NumberOfControllerButtons> Buttons;
			std::array<bool, NumberOfControllerButtons> PreviousButtons;
			std::array<float, NumberOfControllerAxes> Axes;
			std::array<float, NumberOfControllerAxes> PreviousAxes;
		};

		std::array<bool, NumberOfKeys> m_KeyboardKeys = {false};
		std::array<bool, NumberOfKeys> m_PreviousKeyboardKeys = {false};

		std::array<bool, NumberOfMouseButtons> m_MouseButtons = {false};
		std::array<bool, NumberOfMouseButtons> m_PreviousMouseButtons = {false};

		std::int32_t m_MouseX {0};
		std::int32_t m_MouseY {0};
		std::int32_t m_PreviousMouseX {0};
		std::int32_t m_PreviousMouseY {0};
	};
}
