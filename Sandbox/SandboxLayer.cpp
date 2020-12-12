#include "Base.h"
#include "SandboxLayer.h"
#include "Time.h"

namespace DYE
{
    void SandboxLayer::OnEvent(const std::shared_ptr<Event> &pEvent)
    {
        EventDispatcher dispatcher(*pEvent);

        dispatcher.Dispatch<KeyDownEvent>(DYE_BIND_EVENT_FUNCTION(handleOnKeyDown));
        dispatcher.Dispatch<KeyUpEvent>(DYE_BIND_EVENT_FUNCTION(handleOnKeyUp));
    }

    bool SandboxLayer::handleOnKeyDown(const KeyDownEvent &event)
    {
        if (event.GetKeyCode() == KeyCode::Space)
        {
            SDL_Log("You've pressed space!");
        }

        SDL_Log("Sandbox, KeyDown - %d", event.GetKeyCode());
        return true;
    }

    bool SandboxLayer::handleOnKeyUp(const KeyUpEvent &event)
    {
        SDL_Log("Sandbox, KeyUp - %d", event.GetKeyCode());
        return true;
    }

    void SandboxLayer::OnUpdate()
    {
        SDL_Log("DeltaTime - %f", TIME.DeltaTime());
    }

    void SandboxLayer::OnFixedUpdate()
    {
        SDL_Log("FixedDeltaTime - %f", TIME.FixedDeltaTime());
    }
}