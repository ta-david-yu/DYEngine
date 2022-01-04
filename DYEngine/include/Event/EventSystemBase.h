#pragma once

#include "Event.h"

#include <memory>
#include <functional>
#include <vector>

namespace DYE
{
    /// EventSystemBase is the interface for polling events from the underlying frameworks (such as SDL or GLFW), it is designed to be run by an Application object and handled by Application (EventHandler).
    class EventSystemBase
    {
    public:
        virtual ~EventSystemBase() = default;

        virtual void PollEvent() = 0;

        /// Register an event handler, we pass in a raw pointer because the handler itself is responsible for registering & unregistering.
        void Register(EventHandler* handler);

        /// Unregister an event handler, we pass in a raw pointer because the handler itself is responsible for registering & unregistering.
        void Unregister(EventHandler* handler);

        /// The factory function that returns a platform-specific EventSystem instance.
        static std::unique_ptr<EventSystemBase> Create();

    protected:
        std::vector<EventHandler*> m_EventHandlers;

        void broadcast(Event& event);
    };
}