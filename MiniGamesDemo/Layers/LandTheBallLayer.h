#pragma once

#include "Core/LayerBase.h"

#include "ColliderManager.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include "Objects/Camera.h"
#include "Objects/SpriteUnsignedNumber.h"
#include "Objects/LandBall.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace DYE
{
	class WindowBase;

	class LandTheBallLayer : public LayerBase
	{
		enum class GameState
		{
			Preparing, 				// A preparing buffer time to prevent input from the main menu
			WaitingForBallRelease, 	// Wait for player's input to drop the ball
			Playing,
			GameOver
		};

	public:
		explicit LandTheBallLayer(Application& application);
		LandTheBallLayer() = delete;
		LandTheBallLayer(LandTheBallLayer const& other) = delete;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnFixedUpdate() override;
		void OnRender() override;
		void OnImGui() override;

	private:
		void renderSprite(MiniGame::Transform& transform, MiniGame::Sprite& sprite);

		void updateBallWindowPosition();
		void updatePlatformWindowPosition();

		void debugDraw();
		void debugInput();

	private:
		Application& m_Application;

		// Debug settings
		WindowBase* m_pMainWindow = nullptr;
		bool m_DrawImGui = false;
		bool m_DrawDebugGizmos = false;

		// Animation state/settings
		float m_BackgroundScrollingSpeed = 0.0f;

		// UI
		WindowBase* m_pScoreWindow = nullptr;
		MiniGame::SpriteUnsignedNumber m_ScoreNumber;

		// Game world
		GameState m_GameState = GameState::Preparing;
		float m_PreparingTimer = 0.5f;

		glm::vec<2, uint32_t> m_ScreenDimensions;
		std::uint32_t m_ScreenPixelPerUnit = 76;
		bool m_HasGameObjectWindowBeenSetToBordered = false;

		ColliderManager m_ColliderManager;
		MiniGame::Camera m_MainCamera;

		MiniGame::LandBall m_LandBall;
		WindowBase* m_pBallWindow = nullptr;
		MiniGame::Camera m_BallCamera;

		constexpr static float MaxSlowMotionDuration = 0.75f;
		constexpr static float SlowMotionFullyRecoveryTime = 3.0f;
		constexpr static float SlowMotionMultiplier = 0.5f;
		float m_SlowMotionTimer = MaxSlowMotionDuration;
		bool m_ActivateSlowMotion = false;

		constexpr static float MinPlatformX = -10.0f;
		constexpr static float MaxPlatformX = 10.0f;
		constexpr static float PlatformY = -5.0f;
		constexpr static float GameOverY = -15.0f;
		float m_PlatformWidth = 7.0f;
		float m_PlatformHeight = 0.1f;

		WindowBase* m_pPlatformWindow = nullptr;
		float m_PlatformX = 0.0f;

		MiniGame::Transform m_BackgroundTransform;
		MiniGame::Sprite m_BackgroundSprite;

	};
}