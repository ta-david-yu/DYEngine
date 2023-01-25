#pragma once

#include "Core/LayerBase.h"
#include "Event/KeyEvent.h"
#include "Util/FPSCounter.h"

#include "ColliderManager.h"
#include "Objects/Wall.h"
#include "Objects/Camera.h"
#include "Objects/PongPlayer.h"
#include "Objects/PlayerPaddle.h"
#include "Objects/PongBall.h"
#include "Objects/PongHomebase.h"

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
		void renderSprite(MiniGame::Transform& transform, MiniGame::Sprite& sprite);
		/// \return the velocity per second for the paddle.
		void debugInput();
		void readPaddleInput();
		glm::vec2 updatePaddle(MiniGame::PlayerPaddle& paddle, float timeStep);
		void updateBall(float timeStep);
		void updateBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);

		void imguiSprite(const std::string& name, MiniGame::Transform& transform, MiniGame::Sprite& sprite);

	private:
		// Debug settings
		bool m_DrawColliderGizmos = true;
		float m_BackgroundScrollingSpeed = 1.0f;
		FPSCounter m_FPSCounter = FPSCounter(0.25);

		// Meta settings
		WindowBase* m_SecondWindow = nullptr;
		glm::vec2 m_WindowPosition {0, 0};
		float m_TargetWindowWidth = 1600;
		float m_TargetWindowHeight = 900;
		float m_WindowPixelChangePerSecond = 300.0f;

		// Game world
		ColliderManager m_ColliderManager;

		MiniGame::Transform m_BackgroundTransform;
		MiniGame::Sprite m_BackgroundSprite;

		MiniGame::Camera m_MainCamera;
		MiniGame::Camera m_DebugCamera;
		MiniGame::PongBall m_Ball;
		std::vector<MiniGame::PongPlayer> m_Players;
		std::vector<MiniGame::PlayerPaddle> m_PlayerPaddles;
		std::vector<MiniGame::Wall> m_Walls;
		std::vector<MiniGame::PongHomebase> m_Homebases;
	};
}