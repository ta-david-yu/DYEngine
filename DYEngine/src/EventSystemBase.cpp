#include "Events/EventSystemBase.h"
#include "Events/SDLEventSystem.h"

namespace DYE
{
    template<typename T, typename... U>
    static bool EventListenersEqual(std::function<T(U...)> a, std::function<T(U...)> b)
    {
        // TODO: Fixed typeinfo error
        typedef T(*fptr)(U...);
        fptr **pa = a.template target<fptr*>();
        fptr **pb = b.template target<fptr*>();

        return (*pa) == (*pb);

        return false;
    }

    std::unique_ptr<EventSystemBase> EventSystemBase::Create()
    {
        // TODO: add other event system types. For instance, GLFWEventSystem
        return std::make_unique<SDLEventSystem>();
    }

    void EventSystemBase::AddEventListener(EventListener eventListener)
    {
        m_EventListeners.push_back(std::move(eventListener));
    }

    void EventSystemBase::RemoveEventListener(EventListener eventListener)
    {
        /*
        for (auto& listener : m_EventListeners)
        {

        }*/
        for (auto it = m_EventListeners.begin(); it != m_EventListeners.end(); it++)
        {
            if (EventListenersEqual(*it, eventListener))
            {
                m_EventListeners.erase(it);
                break;
            }
        }
    }
}