#pragma once

#include "Event/Event.h"
#include "Input/MouseButton.h"

namespace DYE
{
    class MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(const float x, const float y, const float deltaX, const float deltaY)
            : m_MouseX(x), m_MouseY(y), m_MouseDeltaX(deltaX), m_MouseDeltaY(deltaY)
        {}

        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        float GetDeltaX() const { return m_MouseDeltaX; }
        float GetDeltaY() const { return m_MouseDeltaY; }

        EVENT_CLASS_TYPE(MouseMove)
        EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
    protected:
        float m_MouseX {0}, m_MouseY {0};
        float m_MouseDeltaX {0}, m_MouseDeltaY {0};
    };

    class MouseButtonEvent : public Event
    {
    public:
		MouseButton GetMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
    protected:
        explicit MouseButtonEvent(const MouseButton button)
                : m_Button(button) {}

		MouseButton m_Button;
    };

    class MouseButtonDownEvent : public MouseButtonEvent
    {
    public:
        explicit MouseButtonDownEvent(const MouseButton button)
                : MouseButtonEvent(button) {}

        EVENT_CLASS_TYPE(MouseButtonDown)
    };

    class MouseButtonUpEvent : public MouseButtonEvent
    {
    public:
        explicit MouseButtonUpEvent(const MouseButton button)
                : MouseButtonEvent(button) {}

        EVENT_CLASS_TYPE(MouseButtonUp)
    };
}