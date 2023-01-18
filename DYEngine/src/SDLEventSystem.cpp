#include "Event/SDLEventSystem.h"

#include "Util/Macro.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/GamepadEvent.h"
#include "Graphics/WindowManager.h"

#include <SDL.h>
#include <imgui_impl_sdl.h>

namespace DYE
{
    void SDLEventSystem::PollEvent()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
			// TODO: move this to some place in ImGuiLayer,
			// TODO: so we don't have to know about imgui here.
            // Pass SDL_Event to ImGui
			bool isPreprocessedByImGui = false;
            if (m_PreProcessImGuiEvent)
            {
				isPreprocessedByImGui = ImGui_ImplSDL2_ProcessEvent(&event);
            }

            std::shared_ptr<Event> eventPtr;
			SDL_JoystickID joystickInstanceID;
            bool caught = false;

            switch (event.type)
            {
                case SDL_QUIT:
					eventPtr.reset(new ApplicationQuitEvent(event.quit.timestamp));
                    caught = true;
                    break;
                case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						// We only broadcast window size changed event if it's not an imgui window.
						bool const isOSWindow = WindowManager::HasWindowWithID(event.window.windowID);
						if (!isOSWindow)
						{
							break;
						}

						eventPtr.reset(new WindowSizeChangeEvent(event.window.windowID, event.window.data1, event.window.data2));
						caught = true;
					}
					else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        eventPtr.reset(new WindowManualResizeEvent(event.window.windowID, event.window.data1, event.window.data2));
                        caught = true;
                    }
					else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
					{
						eventPtr.reset(new WindowCloseEvent(event.window.windowID));
						caught = true;
					}
					else if (event.window.event == SDL_WINDOWEVENT_MOVED)
					{
						eventPtr.reset(new WindowMoveEvent(event.window.windowID, event.window.data1, event.window.data2));
						caught = true;
					}

                    /// MORE
                    break;
                case SDL_KEYDOWN:
                    // static case SDL_KeyCode into DYE::KeyCode
                    eventPtr.reset(new KeyDownEvent(static_cast<KeyCode>(event.key.keysym.sym)));
                    caught = true;
                    break;
                case SDL_KEYUP:
                    // static case SDL_KeyCode into DYE::KeyCode
                    eventPtr.reset(new KeyUpEvent(static_cast<KeyCode>(event.key.keysym.sym)));
                    caught = true;
                    break;
                case SDL_MOUSEMOTION:
                    eventPtr.reset(new MouseMovedEvent(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel));
                    caught = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // TODO: Imgui Process, look at imgui_impl_sdl
                    eventPtr.reset(new MouseButtonDownEvent(static_cast<MouseButton>(event.button.button)));
                    caught = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    // TODO: Imgui Process, look at imgui_impl_sdl
                    eventPtr.reset(new MouseButtonUpEvent(static_cast<MouseButton>(event.button.button)));
                    caught = true;
                    break;
				case SDL_CONTROLLERDEVICEADDED:
					// Note that cdevice.which is not the instance id in ControllerAddedEvent, but index (location in the system array).
					// To keep the event data symmetrical, we want to use the instance id instead.
					if (!SDL_GameControllerOpen(event.cdevice.which))
					{
						DYE_LOG_ERROR("SDL_GameControllerOpen failed: %s", SDL_GetError());
						break;
					}
					joystickInstanceID = SDL_JoystickGetDeviceInstanceID(event.cdevice.which);
					eventPtr.reset(new GamepadConnectEvent(joystickInstanceID));
					caught = true;
					break;
				case SDL_CONTROLLERDEVICEREMOVED:
					// This event would only be fired if the disconnected joystick is:
					// 1. a game controller/gamepad
					// 2. has been opened
					joystickInstanceID = event.cdevice.which;
					eventPtr.reset(new GamepadDisconnectEvent(joystickInstanceID));
					caught = true;
					break;
				case SDL_CONTROLLERDEVICEREMAPPED:
					break;
				case SDL_JOYDEVICEADDED:
					joystickInstanceID = SDL_JoystickGetDeviceInstanceID(event.jdevice.which);
					DYE_LOG("Joystick Connect %d (index=%d), count - %d", joystickInstanceID, event.jdevice.which, SDL_NumJoysticks());
					break;
				case SDL_JOYDEVICEREMOVED:
					DYE_LOG("Joystick Disconnect %d, count - %d", event.jdevice.which, SDL_NumJoysticks());
					break;

				default:
                    // Error
                    caught = false;
                    break;
            }

            // Dispatch event
            if (caught)
            {
                broadcast(*eventPtr);
            }
        }
    }
}