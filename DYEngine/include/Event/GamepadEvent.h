#pragma once

#include "Event/Event.h"

#include "Input/Gamepad.h"

namespace DYE
{
	class GamepadEvent : public Event
	{
	public:
		DeviceInstanceID GetDeviceInstanceID() const { return m_InstanceID; }

		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Gamepad)
	protected:
		explicit GamepadEvent(DeviceInstanceID const deviceIndex) : m_InstanceID(deviceIndex) { }
		DeviceInstanceID m_InstanceID;
	};

	class GamepadConnectEvent : public GamepadEvent
	{
	public:
		explicit GamepadConnectEvent(DeviceInstanceID const deviceIndex) : GamepadEvent(deviceIndex) { }

		EVENT_CLASS_TYPE(GamepadConnect)
	};

	class GamepadDisconnectEvent : public GamepadEvent
	{
	public:
		explicit GamepadDisconnectEvent(DeviceInstanceID const deviceIndex) : GamepadEvent(deviceIndex) { }

		EVENT_CLASS_TYPE(GamepadDisconnect)
	};
}