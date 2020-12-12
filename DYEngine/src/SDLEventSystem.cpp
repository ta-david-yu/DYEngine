#include "Events/SDLEventSystem.h"

#include <SDL.h>
#include <imgui_impl_sdl.h>

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"

namespace DYE
{
    void SDLEventSystem::PollEvent()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // Pass SDL_Event to ImGui
            ImGui_ImplSDL2_ProcessEvent(&event);

            std::shared_ptr<Event> eventPtr;
            bool caught = true;
            switch (event.type)
            {
                case SDL_QUIT:
                    eventPtr.reset(new WindowCloseEvent());
                    break;
                case SDL_KEYDOWN:
                    // static case SDL_KeyCode into DYE::KeyCode
                    eventPtr.reset(new KeyDownEvent(static_cast<KeyCode>(event.key.keysym.sym)));
                    break;
                case SDL_KEYUP:
                    // static case SDL_KeyCode into DYE::KeyCode
                    eventPtr.reset(new KeyUpEvent(static_cast<KeyCode>(event.key.keysym.sym)));
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