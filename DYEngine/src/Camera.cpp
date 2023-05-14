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
				DYE_LOG("Try to get the dimension of the render target window but couldn't find the window.");
				return { targetWidth, targetHeight };
			}

			targetWidth = windowPtr->GetWidth();
			targetHeight = windowPtr->GetHeight();
		}
		else
		{
			if (pTargetRenderTexture == nullptr)
			{
				DYE_LOG("Try to get the dimension of the render target but the render target is null.");
				return { targetWidth, targetHeight };
			}

			auto const& properties = pTargetRenderTexture->GetProperties();
			targetWidth = properties.Width;
			targetHeight = properties.Height;
		}

		return { targetWidth, targetHeight };
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		auto viewMatrix = glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(Rotation);
		return glm::inverse(viewMatrix);
	}

	glm::vec2 Camera::ViewportToScreenPoint(glm::vec2 viewportPoint) const
	{
		auto const targetDimensions = Properties.GetTargetDimension();
		Math::Rect const viewportRect = Properties.GetAbsoluteViewportOfDimension(targetDimensions);
		glm::vec2 screenPoint = { viewportRect.X, viewportRect.Y };
		screenPoint += viewportPoint * glm::vec2 { viewportRect.Width, viewportRect.Height };
		return screenPoint;
	}

	glm::vec2 Camera::ScreenToViewportPoint(glm::vec2 screenPoint) const
	{
		auto const targetDimensions = Properties.GetTargetDimension();
		Math::Rect const viewportRect = Properties.GetAbsoluteViewportOfDimension(targetDimensions);
		glm::vec2 const viewportPointInPixels = screenPoint - glm::vec2 { viewportRect.X, viewportRect.Y };
		glm::vec2 const normalizedViewportPoint = viewportPointInPixels / glm::vec2 { viewportRect.Width, viewportRect.Height };
		return normalizedViewportPoint;
	}

	glm::vec3 Camera::ViewportToWorldPoint(glm::vec2 viewportPoint, float distanceFromCameraInWorldUnits) const
	{
		glm::vec2 const targetDimensions = Properties.GetTargetDimension();
		Math::Rect const viewportRect = Properties.GetAbsoluteViewportOfDimension(targetDimensions);
		glm::vec2 const viewportDimensions { viewportRect.Width, viewportRect.Height };

		// The given viewport point's origin is the bottom-left corner,
		// but in camera space the origin should be the center of the screen.
		glm::vec2 pointInProjectionSpace = viewportPoint * 2.0f - glm::vec2(1.0f);
		//pointInProjectionSpace.y = -pointInProjectionSpace.y; // The origin in projection space is the top-left corner.

		auto const projectionMatrix = Properties.GetProjectionMatrix(viewportDimensions.x / viewportDimensions.y);
		auto const viewMatrix = GetViewMatrix();
		glm::mat4 toWorldMatrix = glm::inverse(projectionMatrix * viewMatrix);

		// Near-to-far plane is mapped to -1 ~ 1 in clip space.
		// We want to calculate which value distanceFromCameraInWorldUnits is mapped to.
		float const cameraFrustumDepth = Properties.FarClipDistance - Properties.NearClipDistance;
		float const offsetDistance = distanceFromCameraInWorldUnits - Properties.NearClipDistance;
		float const normalizedDistance = (offsetDistance / cameraFrustumDepth) * 2.0f - 1.0f;
		glm::vec4 pointInWorldSpace = toWorldMatrix * glm::vec4 {pointInProjectionSpace, normalizedDistance, 1};
		pointInWorldSpace /= pointInWorldSpace.w;

		return pointInWorldSpace;
	}

	glm::vec3 Camera::ScreenToWorldPoint(glm::vec2 screenPoint, float distanceFromCameraInWorldUnits) const
	{
		glm::vec2 const targetDimensions = Properties.GetTargetDimension();
		Math::Rect const viewportRect = Properties.GetAbsoluteViewportOfDimension(targetDimensions);
		glm::vec2 const viewportDimensions { viewportRect.Width, viewportRect.Height };

		// The given screen point's origin is the bottom-left corner,
		// but in camera space the origin should be the center of the screen.
		glm::vec2 pointInProjectionSpace = screenPoint - glm::vec2 { viewportRect.X, viewportRect.Y };
		pointInProjectionSpace = (pointInProjectionSpace / viewportDimensions) * 2.0f - glm::vec2(1.0f);
		//pointInProjectionSpace.y = -pointInProjectionSpace.y; // The origin in projection space is the top-left corner.

		auto const projectionMatrix = Properties.GetProjectionMatrix(viewportDimensions.x / viewportDimensions.y);
		auto const viewMatrix = GetViewMatrix();
		glm::mat4 toWorldMatrix = glm::inverse(projectionMatrix * viewMatrix);

		// Near-to-far plane is mapped to -1 ~ 1 in clip space.
		// We want to calculate which value distanceFromCameraInWorldUnits is mapped to.
		float const cameraFrustumDepth = Properties.FarClipDistance - Properties.NearClipDistance;
		float const offsetDistance = distanceFromCameraInWorldUnits - Properties.NearClipDistance;
		float const normalizedDistance = (offsetDistance / cameraFrustumDepth) * 2.0f - 1.0f;
		glm::vec4 pointInWorldSpace = toWorldMatrix * glm::vec4 {pointInProjectionSpace, normalizedDistance, 1};
		pointInWorldSpace /= pointInWorldSpace.w;

		return pointInWorldSpace;
	}
}