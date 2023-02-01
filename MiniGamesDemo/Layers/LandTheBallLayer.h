#pragma once

#include "Core/LayerBase.h"

#include "GizmosRippleEffectManager.h"
#include "WindowParticlesManager.h"

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
			WaitingForBallRelease, 	// Wait for player's input to drop the ball
			Playing,
			GameOver
		};

		enum class Mode
		{
			Normal,
			Windows
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
		void switchToNormalMode();
		void switchToWindowsMode();

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
		bool m_DrawDebugGizmos = true;

		// Animation state/settings
		WindowParticlesManager m_WindowParticlesManager;
		GizmosRippleEffectManager m_GizmosRippleEffectManager;
		float m_BackgroundScrollingSpeed = 0.0f;
		glm::vec<2, uint32_t> m_ScreenDimensions;
		std::uint32_t m_ScreenPixelPerUnit = 76;
		bool m_HasGameObjectWindowBeenSetToBordered = false;

		// UI
		MiniGame::SpriteUnsignedNumber m_ScoreNumber;

		// Game states
		GameState m_GameState = GameState::WaitingForBallRelease;
		Mode m_Mode = Mode::Normal;
		constexpr static int ScoreToSwitchToWindowsMode = 20;

		// Game world
		MiniGame::Camera m_MainCamera;

		// Ball
		MiniGame::LandBall m_LandBall;
		WindowBase* m_pBallWindow = nullptr;
		MiniGame::Camera m_BallCamera;

		// Bullet time
		constexpr static float MaxSlowMotionDuration = 1;
		constexpr static float SlowMotionFullyRecoveryTime = 3.0f;
		constexpr static float SlowMotionMultiplier = 0.5f;
		float m_SlowMotionTimer = MaxSlowMotionDuration;
		bool m_ActivateSlowMotion = false;
		WindowBase* m_pSlowMotionTimerBarWindow = nullptr;

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