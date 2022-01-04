#include "Event/EventSystemBase.h"
#include "Event/SDLEventSystem.h"
#include "Base.h"

namespace DYE
{
    std::unique_ptr<EventSystemBase> EventSystemBase::Create()
    {
        // TODO: add other event system types. For instance, GLFWEventSystem
        return std::make_unique<SDLEventSystem>();
    }

    void EventSystemBase::Register(EventHandler *handler)
    {
        m_EventHandlers.push_back(handler);
#if DYE_DEBUG
        auto registeredHandlerItr = std::find(std::begin(m_EventHandlers), std::end(m_EventHandlers), handler);

        if (registeredHandlerItr != std::end(m_EventHandlers))
        {
            DYE_LOG_WARN("Register a handler that has already been registered.");
        }
#endif
    }

    void EventSystemBase::Unregister(EventHandler *handler)
    {
        auto registeredHandlerItr = std::find(std::begin(m_EventHandlers), std::end(m_EventHandlers), handler);

        if (registeredHandlerItr != std::end(m_EventHandlers))
        {
            m_EventHandlers.erase(registeredHandlerItr);
        }
        else
        {
            DYE_LOG_WARN("Try to unregister a handler that was not registered in the list.");
        }
    }

    void EventSystemBase::broadcast(Event& event)
    {
        for (EventHandler* pHandler : m_EventHandlers)
        {
            pHandler->Handle(event);
        }
    }
}