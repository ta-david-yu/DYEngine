#include "Events/SDLEventSystem.h"

#include <SDL.h>
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"

namespace DYE
{
    void SDLEventSystem::PollEvent()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            std::shared_ptr<Event> eventPtr;
            bool catched = true;
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
                    catched = false;
                    break;
            }

            // Dispatch event
            if (catched)
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