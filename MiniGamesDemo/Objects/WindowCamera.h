#pragma once

#include "Objects/Camera.h"

#include "Math/EasingFunctions.h"

namespace DYE
{
	class WindowBase;
}

namespace DYE::MiniGame
{
	// Window Camera updates the camera properties based on window's location/size, so that the window acts like an in-game camera
	// that moves as the window moves.
	struct WindowCamera
	{
	public:
		enum class AnimationUpdateResult
		{
			 Idle,
			 InProgress,
			 Complete
		};

		Camera Camera;

		glm::vec<2, int> CameraOffset = {0, 0};
		float MoveSpeed = 140;
		float ResizeAnimationDuration = 0.5f;

		WindowBase* GetWindowPtr() { return m_pWindow; }
		void ResetCachedPosition();

		void AssignWindow(WindowBase& window);
		void CreateWindow(std::shared_ptr<ContextBase> contextBase, WindowProperty const& windowProperty);

		void SmoothResize(std::uint32_t width, std::uint32_t height);
		void Translate(glm::vec2 offset);

		AnimationUpdateResult UpdateWindowResizeAnimation(float timeStep);
		void UpdateCameraProperties();

	private:
		WindowBase* m_pWindow = nullptr;

		bool m_IsUpdatingResizeAnimation = false;
		float m_ResizeAnimationTimer = 0.0f;
		float m_OriginalWindowWidth = 1600;
		float m_OriginalWindowHeight = 900;
		float m_TargetWindowWidth = 1600;
		float m_TargetWindowHeight = 900;

		// We need to cache window position with floats so translation offset won't get lost
		// due to integer conversion.
		glm::vec2 m_CachedCurrentPosition = {0, 0};
	};
}