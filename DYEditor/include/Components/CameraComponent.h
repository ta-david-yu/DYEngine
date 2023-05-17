#pragma once

#include "Graphics/Camera.h"

#include "Components/TransformComponent.h"

namespace DYE::DYEditor
{
	struct CameraComponent
	{
		bool IsEnabled = true;
		CameraProperties Properties;

		[[nodiscard]]
		Camera CreateCameraWithTransform(TransformComponent const& transform) const
		{
			Camera camera;
			camera.Position = transform.Position;
			camera.Rotation = transform.Rotation;
			camera.Properties = Properties;

			return camera;
		}
	};
}