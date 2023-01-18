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
		explicit GamepadEvent(DeviceInstanceID const deviceInstanceId) : m_InstanceID(deviceInstanceId) { }
		DeviceInstanceID m_InstanceID;
	};

	class GamepadConnectEvent : public GamepadEvent
	{
	public:
		explicit GamepadConnectEvent(DeviceInstanceID const deviceInstanceId, std::int32_t const deviceIndex) : GamepadEvent(deviceInstanceId), m_DeviceIndex(deviceIndex) { }

		std::int32_t GetDeviceIndex() const { return m_DeviceIndex; }

		EVENT_CLASS_TYPE(GamepadConnect)

	private:
		std::int32_t m_DeviceIndex;
	};

	class GamepadDisconnectEvent : public GamepadEvent
	{
	public:
		explicit GamepadDisconnectEvent(DeviceInstanceID const deviceInstanceId) : GamepadEvent(deviceInstanceId) { }

		EVENT_CLASS_TYPE(GamepadDisconnect)
	};
}