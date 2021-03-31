#include "Events/SDLEventSystem.h"

#include "Base.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include <SDL.h>
#include <imgui_impl_sdl.h>

namespace DYE
{
    void SDLEventSystem::PollEvent()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // Pass SDL_Event to ImGui
            if (m_PreProcessImGuiEvent)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
            }
            std::shared_ptr<Event> eventPtr;
            bool caught = false;

            switch (event.type)
            {
                case SDL_QUIT:
                    eventPtr.reset(new WindowCloseEvent());
                    caught = true;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        eventPtr.reset(new WindowSizeChangeEvent(event.window.data1, event.window.data2));
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
                // Check if EventHandler is referring to a callable target
                if (m_EventHandler)
                {
                    m_EventHandler.operator()(eventPtr);
                }
            }
        }
    }
}