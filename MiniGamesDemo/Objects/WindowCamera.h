#pragma once

#include "Objects/Camera.h"

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

		WindowBase* pWindow = nullptr;
		glm::vec<2, int> CameraOffset = {0, 0};

		void AssignWindow(WindowBase& window);
		void CreateWindow(std::shared_ptr<ContextBase> contextBase, WindowProperty const& windowProperty);

		void SmoothResize(std::uint32_t width, std::uint32_t height);
		void SmoothMove(glm::vec<2, std::int32_t> position);

		AnimationUpdateResult UpdateWindowResizeAnimation(float timeStep);
		AnimationUpdateResult UpdateWindowMoveAnimation(float timeStep);
		void UpdateCameraProperties();

	private:
		bool m_IsUpdatingResizeAnimation = false;
		float m_TargetWindowWidth = 1600;
		float m_TargetWindowHeight = 900;

		bool m_IsUpdatingMoveAnimation = false;
		glm::vec2 m_CachedCurrentPosition = {0, 0};
		glm::vec2 m_TargetPosition = {0, 0};
	};
}