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

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

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

		explicit PongLayer(Application& application);
		PongLayer() = delete;
		PongLayer(PongLayer const& other) = delete;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnFixedUpdate() override;
		void OnRender() override;
		void OnImGui() override;

	private:
		void registerBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		void unregisterBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		void renderSprite(MiniGame::Transform& transform, MiniGame::Sprite& sprite);

		void debugInput();
		void readPlayerInput(float timeStep);
		void updatePaddle(MiniGame::PlayerPaddle& paddle, float timeStep);
		void updateBall(float timeStep);
		void checkIfBallHasReachedGoal(float timeStep);
		void updateBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);

		void imguiSprite(const std::string& name, MiniGame::Transform& transform, MiniGame::Sprite& sprite);

	private:
		Application& m_Application;

		// Debug settings
		WindowBase* m_MainWindow = nullptr;
		bool m_DrawImGui = false;
		bool m_DrawColliderGizmos = false;
		FPSCounter m_FPSCounter = FPSCounter(0.25);

		// Animation state/settings
		float m_BackgroundScrollingSpeed = 0.5f;
		float m_CenterDottedLineScrollingSpeed = 0.4f;

		// Game state
		constexpr static int MaxHealth = 10;
		constexpr static int WindowSizesCount = MaxHealth - 1;
		constexpr static int HealthToEnableWindowInput = 7;
		const std::array<glm::vec<2, std::uint32_t>, WindowSizesCount> HealthWindowSizes =
			{
				glm::vec<2, std::uint32_t>{800, 900},
				glm::vec<2, std::uint32_t>{800, 900},
				glm::vec<2, std::uint32_t>{700, 800},
				glm::vec<2, std::uint32_t>{700, 800},
				glm::vec<2, std::uint32_t>{550, 600},
				glm::vec<2, std::uint32_t>{550, 600},
				glm::vec<2, std::uint32_t>{350, 400},
				glm::vec<2, std::uint32_t>{350, 400},
				glm::vec<2, std::uint32_t>{250, 300}
			};

		GameState m_GameState = GameState::Playing;
		MiniGame::PongPlayer* m_pNextPlayerToSpawnBall = nullptr;
		MiniGame::PlayerPaddle* m_pNextPaddleToSpawnBallAfterIntermission = nullptr;

		// Game world
		ColliderManager m_ColliderManager;

		MiniGame::Transform m_BackgroundTransform;
		MiniGame::Sprite m_BackgroundSprite;

		MiniGame::Transform m_BorderTransform;
		MiniGame::Sprite m_BorderSprite;

		MiniGame::Transform m_CenterLineTransform;
		MiniGame::Sprite m_CenterLineSprite;

		MiniGame::Camera m_MainCamera;
		MiniGame::PongBall m_Ball;
		std::vector<MiniGame::PongPlayer> m_Players;
		std::vector<MiniGame::PlayerPaddle> m_PlayerPaddles;
		std::vector<MiniGame::Wall> m_Walls;
		std::vector<MiniGame::PongHomebase> m_Homebases;

		MiniGame::WindowCamera m_Player1WindowCamera;
		MiniGame::WindowCamera m_Player2WindowCamera;

		// UI related
		MiniGame::Transform m_GameOverUITransform;
		MiniGame::Sprite m_GameOverUISprite;

		MiniGame::Transform m_WinnerUITransform;
		MiniGame::Sprite m_WinnerUISprite;

		std::shared_ptr<Texture2D> m_P1WinsTexture;
		std::shared_ptr<Texture2D> m_P2WinsTexture;
	};
}