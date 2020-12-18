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
            if (m_PreProcessImGuiEvent)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
            }
            auto& io = ImGui::GetIO();

            std::shared_ptr<Event> eventPtr;
            bool caught = false;

            switch (event.type)
            {
                case SDL_QUIT:
                    eventPtr.reset(new WindowCloseEvent());
                    caught = true;
                    break;
                case SDL_KEYDOWN:
                    // static case SDL_KeyCode into DYE::KeyCode
                    eventPtr.reset(new KeyDownEvent(static_cast<KeyCode>(event.key.keysym.sym)));
                    caught = true;
                    break;
                case SDL_KEYUP:
                    if (!(m_PreProcessImGuiEvent && io.WantCaptureKeyboard))
                    {
                        // static case SDL_KeyCode into DYE::KeyCode
                        eventPtr.reset(new KeyUpEvent(static_cast<KeyCode>(event.key.keysym.sym)));
                        caught = true;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // TODO: Imgui Process, look at imgui_impl_sdl
                    break;
                case SDL_MOUSEBUTTONUP:
                    // TODO: Imgui Process, look at imgui_impl_sdl
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