#pragma once

#include "WindowBase.h"
#include "Math/Rect.h"

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
	class Framebuffer;

	enum class RenderTargetType
	{
		Window,
		RenderTexture
	};

	enum class ViewportValueType
	{
		RelativeDimension = 0 	/// The viewport size is calculated based on the target dimensions & relative value.
						  		/// For instance, [window size: (1000, 1000)] + [viewport (0.5, 0.5, 1, 1)] = [actual viewport (500, 500, 1500, 1500)]

		, AbsoluteDimension 	/// The viewport size is set with the given absolute pixel dimensions.
								/// For instance, [viewport (0, 0, 1600, 900)] = [actual viewport (0, 0, 1600, 900)]
	};

	struct CameraProperties
	{
	public:
		glm::vec4 ClearColor = {0, 0, 0, 1};

		/// The vertical field of view of the Camera, in degrees. This is ignored when IsOrthographic is true.
		float FieldOfView = 45;
		bool IsOrthographic = true;
		/// Camera's half-size when in orthographic mode. This is ignored when IsOrthographic is false.
		float OrthographicSize = 10;
		float NearClipDistance = 0.1f;
		float FarClipDistance = 100;

		RenderTargetType TargetType = RenderTargetType::Window;
		std::uint32_t TargetWindowIndex = 0;					/// 0 is normally the main window.
		Framebuffer* pTargetRenderTexture = nullptr;	/// TODO: At some point we will use a reference to RenderTexture instead of Framebuffer.

		/// Cameras with a larger depth value will be drawn on top of cameras with a smaller value.
		float Depth = -1;

		/// The aspect ratio of the render viewport is normally calculated based on render target's size on each render pass automatically.
		/// However you can still set the value manually by setting UseManualAspectRatio to true and assign the value to ManualAspectRatio.
		float CachedAutomaticAspectRatio = 1;

		bool UseManualAspectRatio = false;
		float ManualAspectRatio = 1;

		ViewportValueType ViewportValueType = ViewportValueType::RelativeDimension;
		Math::Rect Viewport {0, 0, 1, 1};

	public:
		bool DoClearColor = true;

	public:
		float GetAspectRatio() const;
		glm::mat4 GetProjectionMatrix(float aspectRatio) const;
		glm::vec<2, std::uint32_t> GetTargetDimension() const;

		Math::Rect GetAbsoluteViewportOfDimension(glm::vec<2, std::uint32_t> targetDimension) const;
		float GetAutomaticAspectRatioOfDimension(glm::vec<2, std::uint32_t> targetDimension) const;
	};

	struct Camera
	{
	public:
		glm::mat4 ViewMatrix {glm::mat4 {1}};
		CameraProperties Properties;

	public:
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);

		glm::quat GetRotation() const;
		void SetRotation(glm::quat rotation);

		/// For a viewport point, the bottom-left of the camera is (0, 0); the top-right is (1, 1).\n
		/// For a screen point, the bottom-left of the screen is (0, 0); the right-top is (screenPixelWidth, screenPixelHeight).\n
		/// Note that a camera viewport doesn't have to cover up a whole screen target (i.e. window/framebuffer),
		/// therefore the corners of the viewport might not be the corners of the screen target.
		glm::vec2 ViewportToScreenPoint(glm::vec2 viewportPoint) const;

		/// For a viewport point, the bottom-left of the camera is (0, 0); the top-right is (1, 1).\n
		/// For a screen point, the bottom-left of the screen is (0, 0); the right-top is (screenPixelWidth, screenPixelHeight).\n
		/// Note that a camera viewport doesn't have to cover up a whole screen target (i.e. window/framebuffer),
		/// therefore the corners of the viewport might not be the corners of the screen target.
		glm::vec2 ScreenToViewportPoint(glm::vec2 screenPoint) const;

		/// For a viewport point, the bottom-left of the camera is (0, 0); the top-right is (1, 1).
		glm::vec3 ViewportToWorldPoint(glm::vec2 viewportPoint, float distanceFromCameraInWorldUnits) const;

		/// For a screen point, the bottom-left of the screen is (0, 0); the right-top is (screenPixelWidth, screenPixelHeight).
		glm::vec3 ScreenToWorldPoint(glm::vec2 screenPoint, float distanceFromCameraInWorldUnits) const;
	};
}