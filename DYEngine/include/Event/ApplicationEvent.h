#pragma once

#include "Event.h"

#include <sstream>

namespace DYE
{
    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_CATEGORY(EventCategory::Application)
        EVENT_CLASS_TYPE(WindowClose)
    };

    class WindowSizeChangeEvent : public Event
    {
    public:
        WindowSizeChangeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowSizeChangeEvent: " << m_Width << ", " << "Height";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowSizeChange)
        EVENT_CLASS_CATEGORY(EventCategory::Application)

    private:
        uint32_t m_Width {0}, m_Height {0};
    };
}