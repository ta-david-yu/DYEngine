#include "Graphics/CameraProperties.h"

namespace DYE
{
	glm::mat4 CameraProperties::GetProjectionMatrix(float aspectRatio) const
	{
		if (IsOrthographic)
		{
			glm::mat4 projectionMatrix = glm::ortho
				(
					-aspectRatio * 0.5f * OrthographicSize,
					aspectRatio * 0.5f * OrthographicSize,
					-0.5f * OrthographicSize,
					0.5f * OrthographicSize,
					NearClipDistance,
					FarClipDistance
				);
			return projectionMatrix;
		}
		else
		{
			glm::mat4 projectionMatrix = glm::perspective
				(
					glm::radians(FieldOfView),
					aspectRatio,
					NearClipDistance,
					FarClipDistance
				);
			return projectionMatrix;
		}
	}
}