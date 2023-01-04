#include "Graphics/CameraProperties.h"

#include "WindowManager.h"
#include "Util/Logger.h"

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

	glm::mat4 CameraProperties::GetViewMatrix() const
	{
		// TODO: Add scale, rotation transformation
		auto const viewMatrix = glm::mat4 {1.0f};
		return glm::translate(viewMatrix, -Position);
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
			auto windowPtr = WindowManager::GetWindowFromID(TargetWindowID);
			if (windowPtr == nullptr)
			{
				return { targetWidth, targetHeight };
			}

			targetWidth = windowPtr->GetWidth();
			targetHeight = windowPtr->GetHeight();
		}
		else
		{
			// TODO: render texture dimension
		}

		return { targetWidth, targetHeight };
	}
}