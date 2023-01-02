#pragma once

#include "Event.h"

#include <sstream>

namespace DYE
{
	class ApplicationQuitEvent : public Event
	{
	public:
		explicit ApplicationQuitEvent(std::uint32_t timeStamp) : m_TimeStamp(timeStamp) { }
		std::uint32_t GetTimeStamp() const { return m_TimeStamp; }

		EVENT_CLASS_TYPE(ApplicationQuit)
		EVENT_CLASS_CATEGORY(EventCategory::Application)

	private:
		std::uint32_t m_TimeStamp;
	};

	class WindowEvent : public Event
	{
	public:
		std::uint32_t GetWindowID() const { return m_WindowID; }
	protected:
		explicit WindowEvent(uint32_t windowID) : m_WindowID(windowID) { }
		std::uint32_t m_WindowID = 0;
	};

    class WindowCloseEvent : public WindowEvent
    {
    public:
        WindowCloseEvent(std::uint32_t windowID) : WindowEvent(windowID) { }

		EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategory::Application)
    };

	class WindowSizeChangeEvent : public WindowEvent
	{
	public:
		WindowSizeChangeEvent(uint32_t windowID, uint32_t width, uint32_t height) : WindowEvent(windowID), m_Width(width), m_Height(height) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowManualResizeEvent: "<< "[" << m_WindowID << "]" << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowSizeChange)
		EVENT_CLASS_CATEGORY(EventCategory::Application)

	private:
		uint32_t m_Width {0}, m_Height {0};
	};

	/// Event that follows WindowSizeChangeEvent if the size was changed by an external event, i.e. the user or the window manager.
    class WindowManualResizeEvent : public WindowEvent
    {
    public:
        WindowManualResizeEvent(uint32_t windowID, uint32_t width, uint32_t height) : WindowEvent(windowID), m_Width(width), m_Height(height) {}

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowManualResizeEvent: "<< "[" << m_WindowID << "]" << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowManualResize)
        EVENT_CLASS_CATEGORY(EventCategory::Application)

    private:
        uint32_t m_Width {0}, m_Height {0};
    };
}