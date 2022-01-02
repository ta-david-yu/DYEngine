#include "Event/EventSystemBase.h"
#include "Event/SDLEventSystem.h"

namespace DYE
{
    std::unique_ptr<EventSystemBase> EventSystemBase::Create()
    {
        // TODO: add other event system types. For instance, GLFWEventSystem
        return std::make_unique<SDLEventSystem>();
    }

    void EventSystemBase::SetEventHandler(EventHandler handler)
    {
        m_EventHandler = std::move(handler);
    }
/*

    template<typename T, typename... U>
    static bool EventHandlersEqual(std::function<T(U...)> a, std::function<T(U...)> b)
    {
        // TODO: Fixed typeinfo error
        typedef T(*fptr)(U...);
        fptr **pa = a.template target<fptr*>();
        fptr **pb = b.template target<fptr*>();

        return (*pa) == (*pb);

        return false;
    }

    void EventSystemBase::AddEventListener(EventHandler handler)
    {
        m_EventListeners.push_back(std::move(handler));
    }

    void EventSystemBase::RemoveEventListener(EventHandler handler)
    {
        for (auto it = m_EventHandlers.begin(); it != m_EventHandlers.end(); it++)
        {
            if (EventHandlersEqual(*it, handler))
            {
                m_EventHandlers.erase(it);
                break;
            }
        }
    }*/
}