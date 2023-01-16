#pragma once

#include "Event/Event.h"

#include "Input/Controller.h"

namespace DYE
{

	class ControllerEvent : public Event
	{
	public:
		DeviceIndex GetDeviceIndex() const { return m_DeviceIndex; }

		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Controller)
	protected:
		explicit ControllerEvent(DeviceIndex const deviceIndex) : m_DeviceIndex(deviceIndex) { }
		DeviceIndex m_DeviceIndex;
	};

	class ControllerConnectEvent : public ControllerEvent
	{
	public:
		// TODO:
		//explicit ControllerConnectEvent( const )
	};
}