#include "Graphics/Camera.h"

#include "Graphics/WindowManager.h"
#include "Graphics/Framebuffer.h"
#include "Util/Logger.h"

#include <glm/gtx/quaternion.hpp>

namespace DYE
{
	float CameraProperties::GetAspectRatio() const
	{
		if (UseManualAspectRatio)
		{
			return ManualAspectRatio;
		}
		else
		{
			return CachedAutomaticAspectRatio;
		}
	}

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

	Math::Rect CameraProperties::GetAbsoluteViewportOfDimension(glm::vec<2, std::uint32_t> targetDimension) const
	{
		if (ViewportValueType == ViewportValueType::AbsoluteDimension)
		{
			return Viewport;
		}
		else
		{
			return {Viewport.X * targetDimension.x, Viewport.Y * targetDimension.y, Viewport.Width * targetDimension.x, Viewport.Height * targetDimension.y};
		}
	}

	float CameraProperties::GetAutomaticAspectRatioOfDimension(glm::vec<2, std::uint32_t> targetDimension) const
	{
		Math::Rect const viewportDimension = GetAbsoluteViewportOfDimension(targetDimension);
		return viewportDimension.Width / viewportDimension.Height;
	}

	glm::vec<2, std::uint32_t> CameraProperties::GetTargetDimension() const
	{
		uint32_t targetWidth = 1;
		uint32_t targetHeight = 1;

		if (TargetType == RenderTargetType::Window)
		{
			auto windowPtr = WindowManager::TryGetWindowAt(TargetWindowIndex);
			if (windowPtr == nullptr)
			{
				return { targetWidth, targetHeight };
			}

			targetWidth = windowPtr->GetWidth();
			targetHeight = windowPtr->GetHeight();
		}
		else
		{
			auto const& properties = pTargetRenderTexture->GetProperties();
			targetWidth = properties.Width;
			targetHeight = properties.Height;
		}

		return { targetWidth, targetHeight };
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		// TODO: Add scale, rotation transformation
		auto viewMatrix = glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(Rotation);
		return glm::inverse(viewMatrix);
	}
}