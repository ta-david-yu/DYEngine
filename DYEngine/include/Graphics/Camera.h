#pragma once

#include "WindowBase.h"
#include "Math/Rect.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
	enum class RenderTargetType
	{
		Window,
		RenderTexture 	/// TODO: To be implemented
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
		glm::vec4 ClearColor = {0, 0, 0, 0};

		/// The vertical field of view of the Camera, in degrees. This is ignored when IsOrthographic is true.
		float FieldOfView = 45;
		bool IsOrthographic = true;
		/// Camera's half-size when in orthographic mode. This is ignored when IsOrthographic is false.
		float OrthographicSize = 10;
		float NearClipDistance = 0.1f;
		float FarClipDistance = 100;

		RenderTargetType TargetType = RenderTargetType::Window;
		WindowID TargetWindowID = 0;

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
		float GetAspectRatio() const;
		glm::mat4 GetProjectionMatrix(float aspectRatio) const;
		glm::vec<2, std::uint32_t> GetTargetDimension() const;

		Math::Rect GetAbsoluteViewportOfDimension(glm::vec<2, std::uint32_t> targetDimension) const;
		float GetAutomaticAspectRatioOfDimension(glm::vec<2, std::uint32_t> targetDimension) const;
	};

	struct Camera
	{
	public:
		glm::vec3 Position {0, 0, 0};
		glm::quat Rotation {glm::vec3 {0, 0, 0}};

		CameraProperties Properties;

	public:
		glm::mat4 GetViewMatrix() const;
	};
}