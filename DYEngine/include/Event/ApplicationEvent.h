#pragma once

#include "Event.h"
#include "Graphics/WindowManager.h"

#include <sstream>
#include <glm/glm.hpp>

namespace DYE
{
    class ApplicationQuitEvent : public Event
    {
    public:
        explicit ApplicationQuitEvent(std::uint32_t timeStamp) : m_TimeStamp(timeStamp) {}
        std::uint32_t GetTimeStamp() const { return m_TimeStamp; }

        EVENT_CLASS_TYPE(ApplicationQuit)
        EVENT_CLASS_CATEGORY(EventCategory::Application)

    private:
        std::uint32_t m_TimeStamp;
    };

    class WindowEvent : public Event
    {
    public:
        WindowID GetWindowID() const { return m_WindowID; }

        EVENT_CLASS_CATEGORY(EventCategory::Application)

    protected:
        explicit WindowEvent(WindowID windowID) : m_WindowID(windowID) {}
        WindowID m_WindowID = 0;
    };

    class WindowCloseEvent : public WindowEvent
    {
    public:
        explicit WindowCloseEvent(WindowID windowID) : WindowEvent(windowID) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowCloseEvent: " << "[" << m_WindowID << "]";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowClose)
    };

    class WindowSizeChangeEvent : public WindowEvent
    {
    public:
        WindowSizeChangeEvent(WindowID windowID, uint32_t width, uint32_t height) : WindowEvent(windowID), m_Width(width), m_Height(height) {}

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowManualResizeEvent: " << "[" << m_WindowID << "] " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowSizeChange)

    private:
        uint32_t m_Width {0}, m_Height {0};
    };

    /// Event that follows WindowSizeChangeEvent if the size was changed by an external event, i.e. the user or the window manager.
    class WindowManualResizeEvent : public WindowEvent
    {
    public:
        WindowManualResizeEvent(WindowID windowID, std::uint32_t width, std::uint32_t height) : WindowEvent(windowID), m_Width(width), m_Height(height) {}

        std::uint32_t GetWidth() const { return m_Width; }
        std::uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowManualResizeEvent: " << "[" << m_WindowID << "] " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowManualResize)

    private:
        std::uint32_t m_Width {0}, m_Height {0};
    };

    class WindowMoveEvent : public WindowEvent
    {
    public:
        WindowMoveEvent(WindowID windowID, std::int32_t x, std::int32_t y) : WindowEvent(windowID), m_X(x), m_Y(y) {}

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMoveEvent: " << "[" << m_WindowID << "] " << m_X << ", " << m_Y;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowMove)

    private:
        std::int32_t m_X {0}, m_Y {0};
    };
}