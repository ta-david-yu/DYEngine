#pragma once

#include <type_traits>
#include <string>

namespace DYE
{
    // TODO: Events are currently blocking, should be using EventQueue later

    /// EventType, described event types
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize,
        KeyDown, KeyUp,
        MouseDown, MouseUp, MouseMove, MouseDrag
    };

    /// EventCategory, a higher level of categories. An event could be Input + Keyboard, or Input + Mouse event
    enum class EventCategory
    {
        None = 0,
        /// Window related events, such as WindowClose, WindowResize
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
    class Event
    {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;

        virtual EventType GetEventType() const = 0;

        /// Return the debug name of the Event
        virtual std::string GetName() const = 0;

        /// Return flags of the category, formed with EventCategory class
        virtual int GetCategoryFlags() const = 0;

        /// Return the debug message for the Event
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category)
        {
            using T = std::underlying_type_t <EventCategory>;
            return GetCategoryFlags() & static_cast<T>(category);
        }

        bool IsUsed() const { return m_IsUsed; }

    protected:
        bool m_IsUsed = false;
    };

    /// The helper class that dispatches the event
    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        ///
        /// \tparam T The type of the Event
        /// \tparam F The type of the event callback function (it is deduced from the parameter by compiler)
        /// \param func The callback function that handles the event
        /// \return return true if the Event type (T) matches up with m_Event, else return false
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.m_IsUsed |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    inline std::ostream & operator<<(std::ostream& os, const Event& evt)
    {
        return os << evt.ToString();
    }
}