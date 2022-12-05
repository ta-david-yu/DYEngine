#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
	struct CameraProperties
	{
	public:
		glm::vec3 Position{0, 0, 0};

		bool IsOrthographic = false;

		/// The vertical field of view of the Camera, in degrees. The value is when IsOrthographic is true.
		float FieldOfView = 45;

		/// Camera's half-size when in orthographic mode. This is ignored when IsOrthographic is false.
		float OrthographicSize = 1;

		float NearClipDistance = 0.1f;
		float FarClipDistance = 100;

	public:

		glm::mat4 GetProjectionMatrix(float aspectRatio) const;

	} __attribute__((aligned(32)));
}