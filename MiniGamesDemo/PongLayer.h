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
#include "Objects/WindowCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
	class WindowBase;

	class PongLayer : public LayerBase
	{
	public:
		enum class GameState
		{
			Playing,
			Intermission, // When someone goaled and playing resizing windows animation.
			GameOver
		};

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
		void readPlayerInput(float timeStep);
		void updatePaddle(MiniGame::PlayerPaddle& paddle, float timeStep);
		void updateBall(float timeStep);
		void checkIfBallHasReachedGoal(float timeStep);
		void updateBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);

		void imguiSprite(const std::string& name, MiniGame::Transform& transform, MiniGame::Sprite& sprite);

	private:
		// Debug settings
		WindowBase* m_MainWindow;
		bool m_DrawColliderGizmos = true;
		float m_BackgroundScrollingSpeed = 1.0f;
		FPSCounter m_FPSCounter = FPSCounter(0.25);

		// Game state
		constexpr static int MaxHealth = 5;
		constexpr static int HealthToEnableWindowInput = 3;
		const std::array<glm::vec<2, std::uint32_t>, MaxHealth - 1> m_HealthWindowSizes =
			{
				glm::vec<2, std::uint32_t>{800, 900},
				glm::vec<2, std::uint32_t>{600, 700},
				glm::vec<2, std::uint32_t>{400, 450},
				glm::vec<2, std::uint32_t>{300, 400}
			};

		GameState m_GameState = GameState::Playing;
		MiniGame::PongPlayer* m_pNextPlayerToSpawnBall = nullptr;
		MiniGame::PlayerPaddle* m_pNextPaddleToSpawnBallAfterIntermission = nullptr;

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

		MiniGame::WindowCamera m_Player1WindowCamera;
		MiniGame::WindowCamera m_Player2WindowCamera;
	};
}