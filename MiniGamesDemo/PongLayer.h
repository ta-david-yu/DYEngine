#pragma once

#include "Core/LayerBase.h"
#include "Event/KeyEvent.h"
#include "Util/FPSCounter.h"

#include "ColliderManager.h"
#include "Objects/PlayerPaddle.h"
#include "Objects/Wall.h"
#include "Objects/Ball.h"
#include "Objects/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
	class WindowBase;

	class PongLayer : public LayerBase
	{
	public:
		PongLayer();
		PongLayer(PongLayer const& other) = delete;

		void OnInit() override;
		void OnEvent(Event& event) override;
		void OnUpdate() override;
		void OnFixedUpdate() override;
		void OnRender() override;
		void OnImGui() override;

	private:
		void registerBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		void unregisterBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		/// \return the velocity per second for the paddle.
		void debugInput();
		void readPaddleInput();
		glm::vec3 updatePaddle(MiniGame::PlayerPaddle& paddle);
		void updateBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		void renderSprite(MiniGame::Transform& transform, MiniGame::Sprite& sprite);

		void imguiSprite(const std::string& name, MiniGame::Transform& transform, MiniGame::Sprite& sprite);

	private:
		// Debug settings
		bool m_DrawColliderGizmos = true;
		float m_BackgroundScrollingSpeed = 1.0f;
		FPSCounter m_FPSCounter = FPSCounter(0.25);

		// Meta settings
		WindowBase* m_SecondWindow = nullptr;
		glm::vec2 m_WindowPosition;
		float m_TargetWindowWidth = 1600;
		float m_TargetWindowHeight = 900;
		float m_WindowPixelChangePerSecond = 300.0f;

		// Game world
		ColliderManager m_ColliderManager;
		std::shared_ptr<CameraProperties> m_CameraProperties;

		MiniGame::Transform m_BackgroundTransform;
		MiniGame::Sprite m_BackgroundSprite;

		MiniGame::Camera m_MainCamera;
		MiniGame::Camera m_DebugCamera;
		MiniGame::Ball m_Ball;
		MiniGame::PlayerPaddle m_PlayerPaddle1;
		MiniGame::PlayerPaddle m_PlayerPaddle2;
		std::vector<MiniGame::Wall> m_Walls;
	};
}