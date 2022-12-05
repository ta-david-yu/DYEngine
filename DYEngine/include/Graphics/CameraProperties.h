#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
	class CameraProperties
	{
	public:
		glm::vec3 Position{0, 0, 0};

		/// The vertical field of view of the Camera, in degrees. The value is when IsOrthographic is true.
		float FieldOfView = 45;

		/// Camera's half-size when in orthographic mode. This is ignored when IsOrthographic is false.
		float OrthographicSize = 1;

		float NearClipDistance = 0.1f;
		float FarClipDistance = 100;

		bool IsOrthographic = false;

	public:
		glm::mat4 GetProjectionMatrix(float aspectRatio) const;
	};
}