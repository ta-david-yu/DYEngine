#pragma once

#include "Message/Messaging.h"

#include <string>
#include <type_traits>

namespace DYE
{
    // TODO: Event are currently blocking, should be using EventQueue later

    /// EventType, described event types
    enum class EventType
    {
        None = 0,
        WindowClose, WindowSizeChange,
        KeyDown, KeyUp,
        MouseButtonDown, MouseButtonUp, MouseMove, MouseDrag
    };

    /// EventCategory, a higher level of categories. An event could be Input + Keyboard, or Input + Mouse event
    enum class EventCategory
    {
        None = 0,
        /// Window related events, such as WindowClose, WindowSizeChange
        Application =  1<<0,
        Input =        1<<1,
        Keyboard =     1<<2,
        Mouse =        1<<3
    };

    inline EventCategory operator | (EventCategory lhs, EventCategory rhs)
    {
        using T = std::underlying_type_t <EventCategory>;
        return static_cast<EventCategory>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    /// Every derived class should have this.
    /// \param category: EventCategory. For instance, EventCategory::Input | EventCategory::Keyboard
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { \
                                        using T = std::underlying_type_t <EventCategory>; \
                                        return static_cast<T>(category); }\

    /// Every derived final class should have this.
    /// \param type: EventType. For instance, EventType::KeyDown
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual std::string GetName() const override { return #type; }\

    /// Event data structure
    class Event : MessageData
    {
    public:
        ~Event() override = default;

        virtual EventType GetEventType() const = 0;

        /// Return flags of the category, formed with EventCategory class
        virtual int GetCategoryFlags() const = 0;

        /// Return the debug message for the Event
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) const
        {
            using T = std::underlying_type_t <EventCategory>;
            return GetCategoryFlags() & static_cast<T>(category);
        }

        bool IsUsed = false;
    };

    inline std::ostream & operator<<(std::ostream& os, const Event& evt)
    {
        return os << evt.ToString();
    }

    /// EventHandleFunction handles all the Events emit by an EventSystemBase
    class EventHandler : MessageHandlerBase<Event>
    {
    public:
        void Handle(Event& event) override = 0;
    };
}