#pragma once

#include "Event.h"

#include <memory>
#include <functional>
#include <vector>

namespace DYE
{
    /// EventListener is a function call that takes an Event as the argument
    /// \return true if the event is used, false if not
    using EventListener = std::function<bool(std::shared_ptr<Event>)>;

    class EventSystemBase
    {
    public:
        // ctor and dtor
        virtual ~EventSystemBase() = default;

        virtual void PollEvent() = 0;

        void AddEventListener(EventListener eventListener);

        void RemoveEventListener(EventListener eventListener);

        static std::unique_ptr<EventSystemBase> Create();

    protected:
        // TODO: use a unordered-map list for adding interested event only
        std::vector<EventListener> m_EventListeners;
    };
}