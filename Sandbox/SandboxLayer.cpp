#include "SandboxLayer.h"

namespace DYE
{
    void SandboxLayer::OnEvent(const std::shared_ptr<Event> &pEvent)
    {
        auto eventType = pEvent->GetEventType();

        switch (eventType)
        {
            case EventType::KeyDown:
                break;
            case EventType::KeyUp:
                break;
            default:
                break;
        }
    }
}