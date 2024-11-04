#pragma once

#include "Event/Event.h"
#include "Input/KeyCode.h"

#include <sstream>

namespace DYE
{
    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
    protected:
        explicit KeyEvent(const KeyCode keyCode) : m_KeyCode(keyCode) {}
        KeyCode m_KeyCode;
    };

    class KeyDownEvent : public KeyEvent
    {
    public:
        explicit KeyDownEvent(const KeyCode keyCode) : KeyEvent(keyCode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyDownEvent: " << "[" << GetKeyName(m_KeyCode) << "]";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyDown)
    };

    class KeyUpEvent : public KeyEvent
    {
    public:
        explicit KeyUpEvent(const KeyCode keyCode) : KeyEvent(keyCode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyUpEvent: " << "[" << GetKeyName(m_KeyCode) << "]";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyUp)
    };
}