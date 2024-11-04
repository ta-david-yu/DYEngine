#pragma once

#include "Message/Messaging.h"
#include "Event/Event.h"

#include <memory>
#include <functional>
#include <vector>

namespace DYE
{
    /// EventSystemBase is the interface for polling system events from the underlying frameworks (such as SDL or GLFW).
    /// It is designed to be run by an Application object and handled by Application (EventHandler).
    class EventSystemBase : public MessageDispatcherBase<Event>
    {
    public:
        virtual ~EventSystemBase() = default;

        virtual void PollEvent() = 0;

        /// The factory function that returns a platform-specific EventSystem instance.
        static std::unique_ptr<EventSystemBase> Create();

    protected:
        std::vector<EventHandler *> m_EventHandlers {};
    };
}