#include "Event/SDLEventSystem.h"

#include "Base.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "WindowManager.h"

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
            bool caught = false;

            switch (event.type)
            {
                case SDL_QUIT:
                    eventPtr.reset(new WindowCloseEvent(event.window.windowID));
                    caught = true;
                    break;
                case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						// We only broadcast window size changed event if it's not an imgui window.
						// TODO: call
						//  'isOSWindow = WindowManager.GetWindowFromID(event.window.windowID) != nullptr'
						//  for now we will just call
						//  'isOSWindow = event.window.windowID == 1' (the main OS Window always has the ID of 1)
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
						// TODO: implement window close event
						DYE_LOG_INFO("WINDOW_CLOSE (main window) to be handled!");

						if (event.window.windowID == 1)
						{
							// TODO: remove this
							eventPtr.reset(new WindowCloseEvent(event.window.windowID));
							caught = true;
						}
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
                    eventPtr.reset(new MouseButtonDownEvent(static_cast<MouseCode>(event.button.button)));
                    caught = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    // TODO: Imgui Process, look at imgui_impl_sdl
                    eventPtr.reset(new MouseButtonUpEvent(static_cast<MouseCode>(event.button.button)));
                    caught = true;
                    break;
                default:
                    // Error
                    //SDL_Log("Unhandled SDL Event Type %d", event.type);
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