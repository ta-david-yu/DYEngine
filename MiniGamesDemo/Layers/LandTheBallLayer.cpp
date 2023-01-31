#include "Layers/LandTheBallLayer.h"

#include "MiniGamesApp.h"

#include "Core/Application.h"
#include "Util/Logger.h"
#include "Util/Macro.h"
#include "Util/Time.h"
#include "ImGui/ImGuiUtil.h"
#include "Graphics/DebugDraw.h"
#include "Input/InputManager.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"
#include "Screen.h"

#include <imgui.h>
#include <glm/gtc/random.hpp>

namespace DYE
{
	LandTheBallLayer::LandTheBallLayer(Application &application) : m_Application(application)
	{

	}

	void LandTheBallLayer::OnAttach()
	{
		// Set main window location and size.
		m_pMainWindow = WindowManager::GetMainWindow();
		m_pMainWindow->SetSize(1600, 900);
		m_pMainWindow->CenterWindow();
		m_pMainWindow->Restore();

		// Initialize screen parameters.
		auto displayMode = Screen::GetInstance().GetDisplayMode(0);
		m_ScreenDimensions = {displayMode->Width, displayMode->Height };

		// Score number UI.
		m_ScoreNumber.Transform.Scale = {6, 6, 1};
		m_ScoreNumber.Transform.Position = {0, 1, 0};
		m_ScoreNumber.DigitDistanceOffset = 0.5f;
		m_ScoreNumber.LoadTexture();
		m_ScoreNumber.SetValue(0);

		m_pScoreWindow = WindowManager::CreateWindow(WindowProperty("Score", 256, 96));
		m_pScoreWindow->SetSize(256, 128);
		m_pScoreWindow->SetContext(m_pMainWindow->GetContext());
		m_pScoreWindow->CenterWindow();
		auto scoreWindowPosition = m_pScoreWindow->GetPosition();
		scoreWindowPosition.y = 30.0f;
		m_pScoreWindow->SetPosition(scoreWindowPosition);

		// Create game objects.
		m_LandBall.Transform.Position = {0, 0, 0};
		m_LandBall.Velocity.Value = {0, 0};

		m_pBallWindow = WindowManager::CreateWindow(WindowProperty("Ball"));
		m_pBallWindow->SetContext(m_pMainWindow->GetContext());
		m_pBallWindow->SetSize(m_ScreenPixelPerUnit, m_ScreenPixelPerUnit);
		m_pBallWindow->CenterWindow();
		m_pBallWindow->SetBorderedIfWindowed(false);

		m_pPlatformWindow = WindowManager::CreateWindow(WindowProperty("Platform"));
		m_pPlatformWindow->SetSize(m_ScreenPixelPerUnit * m_PlatformWidth, m_ScreenPixelPerUnit * m_PlatformHeight);
		m_pPlatformWindow->CenterWindow();
		m_pPlatformWindow->SetBorderedIfWindowed(false);

		// Create background object.
		m_BackgroundSprite.Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundSprite.Texture->PixelsPerUnit = 32;
		m_BackgroundSprite.IsTiled = true;
		m_BackgroundTransform.Scale = {64.0f, 64.0f, 1};
		m_BackgroundTransform.Position = {0, 0, -2};

		// Set camera properties
		m_MainCamera.Transform.Position = glm::vec3 {0, 0, 10};
		m_MainCamera.Properties.IsOrthographic = true;
		m_MainCamera.Properties.OrthographicSize = 12;
		m_MainCamera.Properties.TargetType = RenderTargetType::Window;
		m_MainCamera.Properties.TargetWindowID = m_pMainWindow->GetWindowID();
		m_MainCamera.Properties.UseManualAspectRatio = false;
		m_MainCamera.Properties.ManualAspectRatio = (float) 1600 / 900;
		m_MainCamera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		m_MainCamera.Properties.Viewport = { 0, 0, 1, 1 };

		m_BallCamera = m_MainCamera;
		m_BallCamera.Properties.TargetWindowID = m_pBallWindow->GetWindowID();


		// Hide the main window by default
		m_pMainWindow->Minimize();
		updatePlatformWindowPosition();
	}

	void LandTheBallLayer::OnDetach()
	{
		WindowManager::CloseWindow(m_pScoreWindow->GetWindowID());
		WindowManager::CloseWindow(m_pBallWindow->GetWindowID());
		WindowManager::CloseWindow(m_pPlatformWindow->GetWindowID());
	}

	void LandTheBallLayer::OnUpdate()
	{
		if (m_DrawDebugGizmos)
		{
			debugDraw();
		}

		debugInput();

		// We delay the set window border call here to avoid weird window bug.
		if (!m_HasGameObjectWindowBeenSetToBordered)
		{
			m_HasGameObjectWindowBeenSetToBordered = true;
			m_pBallWindow->SetBorderedIfWindowed(true);
			m_pPlatformWindow->SetBorderedIfWindowed(true);
		}

		if (m_GameState == GameState::Preparing)
		{
			m_PreparingTimer -= TIME.DeltaTime();
			if (m_PreparingTimer > 0.0f)
			{
				return;
			}

			// Time is up. Go to the next state.
			m_GameState = GameState::WaitingForBallRelease;
		}

		// In-game input
		if (m_GameState == GameState::GameOver)
		{
			// Game over, no more in-game input.
			return;
		}

		// Keyboard
		if (m_GameState == GameState::WaitingForBallRelease &&
			(INPUT.GetKeyDown(KeyCode::Return) || INPUT.GetKeyDown(KeyCode::Space)))
		{
			// Release the ball.
			m_GameState = GameState::Playing;
		}

		if (INPUT.GetKey(KeyCode::Right) || INPUT.GetKey(KeyCode::D))
		{
			m_LandBall.HorizontalMovementBuffer = 1;
		}
		else if (INPUT.GetKey(KeyCode::Left) || INPUT.GetKey(KeyCode::A))
		{
			m_LandBall.HorizontalMovementBuffer = -1;
		}

		// Controller
		if (!INPUT.IsGamepadConnected(0))
		{
			// Controller not connected, skip it.
			return;
		}

		if (m_GameState == GameState::WaitingForBallRelease && INPUT.GetGamepadButton(0, GamepadButton::South))
		{
			// Release the ball.
			m_GameState = GameState::Playing;
		}

		float const horizontal = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickHorizontal);
		if (glm::abs(horizontal) > 0.1f)
		{
			// Manual dead zone (to avoid analog stick drifting).
			m_LandBall.HorizontalMovementBuffer = horizontal;
		}

		if (INPUT.GetGamepadButton(0, GamepadButton::DPadRight))
		{
			m_LandBall.HorizontalMovementBuffer = 1;
		}
		else if (INPUT.GetGamepadButton(0, GamepadButton::DPadLeft))
		{
			m_LandBall.HorizontalMovementBuffer = -1;
		}
	}

	void LandTheBallLayer::debugDraw()
	{
		DebugDraw::Cube({m_PlatformX, PlatformY - m_LandBall.Transform.Scale.y * 0.5f, 0}, {m_PlatformWidth, m_PlatformHeight, 1}, Color::Blue);
		DebugDraw::Cube(m_LandBall.Transform.Position, m_LandBall.Transform.Scale, Color::Yellow);
	}

	void LandTheBallLayer::debugInput()
	{
		if (INPUT.GetKeyDown(KeyCode::F9))
		{
			m_DrawImGui = !m_DrawImGui;
		}

		if (INPUT.GetKeyDown(KeyCode::Escape))
		{
			auto &miniGamesApp = static_cast<MiniGamesApp &>(m_Application);
			miniGamesApp.LoadMainMenuLayer();
		}
	}

	void LandTheBallLayer::OnFixedUpdate()
	{
		if (m_GameState != GameState::Playing)
		{
			// Skip any ball movement if it's not in Playing state
			return;
		}

		float const timeStep = TIME.FixedDeltaTime();

		m_LandBall.Velocity.Value.x = m_LandBall.HorizontalMovementBuffer * m_LandBall.HorizontalMoveSpeed;
		m_LandBall.HorizontalMovementBuffer = 0.0f;
		m_LandBall.Velocity.Value.y += m_LandBall.GetGravity() * timeStep;

		float const xChange = m_LandBall.Velocity.Value.x * timeStep;
		float const yChange = m_LandBall.Velocity.Value.y * timeStep;

		float const prevBallY = m_LandBall.Transform.Position.y;
		float newBallY = m_LandBall.Transform.Position.y + yChange;
		float const newBallX = m_LandBall.Transform.Position.x + xChange;

		if (newBallY < GameOverY)
		{
			m_GameState = GameState::GameOver;
			m_pMainWindow->Restore();
		}
		else if (prevBallY >= PlatformY && newBallY < PlatformY)
		{
			// Reach the platform height! Check if the ball hits the platform.
			float const minX = m_PlatformX - m_PlatformWidth * 0.5f;
			float const maxX = m_PlatformX + m_PlatformWidth * 0.5f;

			float const ballMinX = newBallX - m_LandBall.Transform.Scale.x * 0.5f;
			float const ballMaxX = newBallX + m_LandBall.Transform.Scale.x * 0.5f;

			bool const noOverlap = (maxX < ballMinX || ballMaxX < minX);
			if (!noOverlap)
			{
				// Calculate new vertical launch speed and apply it.

				// Move platform.
				float newPlatformX = glm::linearRand(MinPlatformX, MaxPlatformX);
				float const currentPlatformLeft = m_PlatformX - m_PlatformWidth * 0.5f;
				float const currentPlatformRight = m_PlatformX + m_PlatformWidth * 0.5f;

				int numberOfTriesAvailable = 2;

				while (numberOfTriesAvailable > 0 &&
					   newPlatformX >= currentPlatformLeft && newPlatformX <= currentPlatformRight)
				{
					// The new platform position is located within the current platform bound,
					// Re-generate one to reduce the number of similar platform spawns.
					newPlatformX = glm::linearRand(MinPlatformX, MaxPlatformX);
					numberOfTriesAvailable--;
				}
				m_PlatformX = newPlatformX;

				// TODO: Play VFX/animation etc.
				m_LandBall.OnBounce();
				m_ScoreNumber.SetValue(m_ScoreNumber.GetValue() + 1);

				m_LandBall.Velocity.Value.y = m_LandBall.GetLaunchVerticalSpeed();
				newBallY = PlatformY;
			}
		}

		m_LandBall.Transform.Position.x = newBallX;
		m_LandBall.Transform.Position.y = newBallY;

		updateBallWindowPosition();
		updatePlatformWindowPosition();
	}

	void LandTheBallLayer::updateBallWindowPosition()
	{
		// Move ball window to match the location of the real ball.
		std::int32_t ballScreenPositionX = m_LandBall.Transform.Position.x * m_ScreenPixelPerUnit;
		std::int32_t ballScreenPositionY = m_LandBall.Transform.Position.y * m_ScreenPixelPerUnit;
		ballScreenPositionY = m_ScreenDimensions.y - ballScreenPositionY;

		// Offset based on the size of the window ball (because the origin of a window is at top-left corner).
		ballScreenPositionX -= m_ScreenPixelPerUnit * m_LandBall.Transform.Scale.x * 0.5f;
		ballScreenPositionY -= m_ScreenPixelPerUnit * m_LandBall.Transform.Scale.y * 0.5f;

		// Offset based on screen dimensions (because screen space origin is at top-left corner).
		ballScreenPositionX += m_ScreenDimensions.x * 0.5f;
		ballScreenPositionY -= m_ScreenDimensions.y * 0.5f;

		m_pBallWindow->SetPosition(ballScreenPositionX, ballScreenPositionY);
	}

	void LandTheBallLayer::updatePlatformWindowPosition()
	{
		// Move platform window to match the location of the real platform.
		std::int32_t platformScreenPositionX = m_PlatformX * m_ScreenPixelPerUnit;
		std::int32_t platformScreenPositionY = PlatformY * m_ScreenPixelPerUnit;
		platformScreenPositionY = m_ScreenDimensions.y - platformScreenPositionY;

		// Offset based on the size of the platform (because the origin of a window is at top-left corner).
		platformScreenPositionX -= m_ScreenPixelPerUnit * m_PlatformWidth * 0.5f;

		// Offset based on screen dimensions (because screen space origin is at top-left corner).
		platformScreenPositionX += m_ScreenDimensions.x * 0.5f;
		platformScreenPositionY -= m_ScreenDimensions.y * 0.5f;

		// Lower the window a bit so the top of the window (border/menubar) matches with the real platform
		// Yeah, magic number, sorry :P
		platformScreenPositionY += m_ScreenPixelPerUnit * 0.5;

		m_pPlatformWindow->SetPosition(platformScreenPositionX, platformScreenPositionY);
	}

	void LandTheBallLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(m_MainCamera.GetTransformedProperties());
		RenderPipelineManager::RegisterCameraForNextRender(m_BallCamera.GetTransformedProperties());

		// Scroll background texture.
		float const backgroundTilingOffsetChange = TIME.DeltaTime() * m_BackgroundScrollingSpeed;
		m_BackgroundSprite.TilingOffset += glm::vec2 {backgroundTilingOffsetChange, backgroundTilingOffsetChange};
		if (m_BackgroundSprite.TilingOffset.x > 1.0f)
		{
			m_BackgroundSprite.TilingOffset -= glm::vec2 {1.0f, 1.0f};
		}
		renderSprite(m_BackgroundTransform, m_BackgroundSprite);

		m_ScoreNumber.Render();
	}

	void LandTheBallLayer::renderSprite(MiniGame::Transform &transform, MiniGame::Sprite &sprite)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, transform.Position);
		modelMatrix = modelMatrix * glm::toMat4(transform.Rotation);
		modelMatrix = glm::scale(modelMatrix, transform.Scale);

		if (sprite.IsTiled)
		{
			RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
				->SubmitTiledSprite(sprite.Texture, {transform.Scale.x * sprite.TilingScale.x, transform.Scale.y * sprite.TilingScale.y, sprite.TilingOffset}, sprite.Color, modelMatrix);
		}
		else
		{
			RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
				->SubmitSprite(sprite.Texture, sprite.Color, modelMatrix);
		}
	}

	void LandTheBallLayer::OnImGui()
	{
		if (!m_DrawImGui)
		{
			return;
		}

		if (ImGui::Begin("Menu"))
		{
			if (ImGui::CollapsingHeader("Load", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto &miniGamesApp = static_cast<MiniGamesApp &>(m_Application);
				if (ImGui::Button("Main Menu"))
				{
					miniGamesApp.LoadMainMenuLayer();
				}
				ImGui::SameLine();
				if (ImGui::Button("Pong"))
				{
					miniGamesApp.LoadPongLayer();
				}
				ImGui::SameLine();
				if (ImGui::Button("Land Ball"))
				{
					miniGamesApp.LoadLandBallLayer();
				}
			}

			if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Button("Toggle Gizmos"))
				{
					m_DrawDebugGizmos = !m_DrawDebugGizmos;
				}
			}

			if (ImGui::CollapsingHeader("Game State", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto value = m_ScoreNumber.GetValue();

				if (ImGui::Button("-3"))
				{
					if (value > 0)
					{
						value--;
					}
					m_ScoreNumber.SetValue(value);
				}

				ImGui::SameLine();
				if (ImGui::Button("+3"))
				{
					value++;
					m_ScoreNumber.SetValue(value);
				}

				ImGuiUtil::DrawFloatControl("Time To Reach Apex", m_LandBall.TimeToReachApex, MiniGame::LandBall::InitialTimeToReachApex);
				ImGuiUtil::DrawFloatControl("Percentage Loss Per Bounce", m_LandBall.TimePercentageLossPerBounce, 0.015f);
				ImGuiUtil::DrawFloatControl("Minimum Time To Reach Apex", m_LandBall.MinimumTimeToReachApex, 0.4f);

				ImGuiUtil::DrawReadOnlyTextWithLabel("Current Gravity", std::to_string(m_LandBall.GetGravity()));

				ImGui::Spacing();
				static bool isBallWindowBorderless = true;
				if (ImGui::Button("Ball Borderless"))
				{
					m_pBallWindow->SetBorderedIfWindowed(!isBallWindowBorderless);
					isBallWindowBorderless = !isBallWindowBorderless;
				}
			}
		}
		ImGui::End();
	}
}