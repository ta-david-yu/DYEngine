#pragma once

#include "Event.h"

#include <memory>
#include <functional>
#include <vector>

namespace DYE
{
    /// EventHandler is a function call that takes an Event as the argument
    /// \return true if the event is used, false if not
    using EventHandler = std::function<bool(std::shared_ptr<Event>)>;

    /// EventSystemBase is the interface for polling events from the underlying frameworks (such as SDL or GLFW), it is designed to be run by an Application object and handled by Application EventHandler
    class EventSystemBase
    {
    public:
        // ctor and dtor
        virtual ~EventSystemBase() = default;

        virtual void PollEvent() = 0;

        void SetEventHandler(EventHandler handler);
        /*
        void AddEventListener(EventHandler handler);

        void RemoveEventListener(EventHandler handler);
        */
        static std::unique_ptr<EventSystemBase> Create();

    protected:
        // TODO: use a unordered-map list for adding interested event only
        //std::vector<EventHandler> m_EventHandlers;

        EventHandler m_EventHandler;
    };
}