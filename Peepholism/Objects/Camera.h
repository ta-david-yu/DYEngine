#pragma once

#include "Graphics/CameraProperties.h"

#include "Components/Transform.h"

namespace DYE::Pong
{
	struct Camera
	{
		Transform Transform;
		CameraProperties Properties;

		CameraProperties GetTransformedProperties() const
		{
			CameraProperties properties = Properties;
			properties.Position = Transform.Position;
			return properties;
		}
	};
}