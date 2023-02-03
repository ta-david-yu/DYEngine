#include "Layers/MainMenuLayer.h"

#include "MiniGamesApp.h"
#include "LandTheBallLayer.h"

#include "Core/Application.h"
#include "Util/Logger.h"
#include "Util/Macro.h"
#include "Util/Time.h"
#include "Input/InputManager.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"

#include "imgui.h"

namespace DYE
{
	MainMenuLayer::MainMenuLayer(Application &application) : m_Application(application)
	{

	}

	void MainMenuLayer::OnAttach()
	{
		LandTheBallLayer::LoadHighScore();

		// Create background object.
		m_BackgroundSprite.Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundSprite.Texture->PixelsPerUnit = 32;
		m_BackgroundSprite.IsTiled = true;
		m_BackgroundTransform.Scale = {64.0f, 64.0f, 1};
		m_BackgroundTransform.Position = {0, 0, -2};

		// Create UI objects.
		m_LogoSprite.Texture = Texture2D::Create("assets\\Sprite_DYELogo.png");
		m_LogoSprite.Texture->PixelsPerUnit = 12;
		m_LogoTransform.Position = {-6, 3.0f, 0};

		m_ButtonPromptSprite.Texture = Texture2D::Create("assets\\Sprite_MenuButtonPrompt.png");
		m_ButtonPromptSprite.Texture->PixelsPerUnit = 32;
		m_ButtonPromptTransform.Position = {-5, -6.0f, 0};

		m_LandTheBallSubtitleTexture = Texture2D::Create("assets\\Sprite_RuleLandTheBall.png");
		m_LandTheBallSubtitleTexture->PixelsPerUnit = 20;

		m_PongSubtitleTexture = Texture2D::Create("assets\\Sprite_RulePong.png");
		m_PongSubtitleTexture->PixelsPerUnit = 20;

		m_ExitSubtitleTexture = Texture2D::Create("assets\\Sprite_SubtitleExit.png");
		m_ExitSubtitleTexture->PixelsPerUnit = 20;

		m_SubtitleSprite.Texture = m_LandTheBallSubtitleTexture;
		m_SubtitleTransform.Position = {5, -3, 0};

		MiniGame::SpriteButton landTheBallButton;
		landTheBallButton.Transform.Position = {-5, -1.5f, 0};
		landTheBallButton.SelectedTexture = Texture2D::Create("assets\\Sprite_ButtonLandTB_Selected.png");
		landTheBallButton.SelectedTexture->PixelsPerUnit = 32;
		landTheBallButton.DeselectedTexture = Texture2D::Create("assets\\Sprite_ButtonLandTB_Deselected.png");
		landTheBallButton.DeselectedTexture->PixelsPerUnit = 32;
		landTheBallButton.SetDeselectAppearance();
		m_MenuButtons.push_back(std::move(landTheBallButton));

		MiniGame::SpriteButton pongButton;
		pongButton.Transform.Position = {-5, -3, 0};
		pongButton.SelectedTexture = Texture2D::Create("assets\\Sprite_ButtonPong_Selected.png");
		pongButton.SelectedTexture->PixelsPerUnit = 32;
		pongButton.DeselectedTexture = Texture2D::Create("assets\\Sprite_ButtonPong_Deselected.png");
		pongButton.DeselectedTexture->PixelsPerUnit = 32;
		pongButton.SetDeselectAppearance();
		m_MenuButtons.push_back(std::move(pongButton));

		MiniGame::SpriteButton exitButton;
		exitButton.Transform.Position = {-5, -4.5f, 0};
		exitButton.SelectedTexture = Texture2D::Create("assets\\Sprite_ButtonExit_Selected.png");
		exitButton.SelectedTexture->PixelsPerUnit = 32;
		exitButton.DeselectedTexture = Texture2D::Create("assets\\Sprite_ButtonExit_Deselected.png");
		exitButton.DeselectedTexture->PixelsPerUnit = 32;
		exitButton.SetDeselectAppearance();
		m_MenuButtons.push_back(std::move(exitButton));

		// Initialize high score.
		m_HighScoreTextTransform.Position = {1.2f, -6, 0};
		m_HighScoreTextSprite.Texture = Texture2D::Create("assets\\Sprite_HighScoreText.png");
		m_HighScoreTextSprite.Texture->PixelsPerUnit = 32;

		m_LandTheBallHighScoreNumber.Transform.Position = {2.5f, -6, 0};
		m_LandTheBallHighScoreNumber.Transform.Scale = {0.5f, 0.5f, 1};
		m_LandTheBallHighScoreNumber.DigitDistanceOffset = 0.5f;
		m_LandTheBallHighScoreNumber.LoadTexture();
		m_LandTheBallHighScoreNumber.SetValue(LandTheBallLayer::HighScore);

		// Set window location and size.
		m_MainWindow = WindowManager::GetMainWindow();
		m_MainWindow->SetSize(1600, 900);
		m_MainWindow->CenterWindow();
		m_MainWindow->Restore();

		// Set camera properties
		m_MainCamera.Transform.Position = glm::vec3 {0, 0, 10};
		m_MainCamera.Properties.IsOrthographic = true;
		m_MainCamera.Properties.OrthographicSize = 15;
		m_MainCamera.Properties.TargetType = RenderTargetType::Window;
		m_MainCamera.Properties.TargetWindowID = m_MainWindow->GetWindowID();
		m_MainCamera.Properties.UseManualAspectRatio = false;
		m_MainCamera.Properties.ManualAspectRatio = (float) 1600 / 900;
		m_MainCamera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		m_MainCamera.Properties.Viewport = { 0, 0, 1, 1 };

		// Set initial selected button index: 0
		m_SelectedButtonIndex = 0;
		m_MenuButtons[m_SelectedButtonIndex].SetSelectAppearance();
	}

	void MainMenuLayer::registerBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
	{
		if (collider.ID.has_value() && m_ColliderManager.IsColliderRegistered(collider.ID.value()))
		{
			// The collider has already been registered to the manager.
			return;
		}

		collider.ID = m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter(transform.Position, collider.Size));
	}

	void MainMenuLayer::unregisterBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
	{
		if (!collider.ID.has_value())
		{
			// The collider doesn't have an ID.
			return;
		}

		if (!m_ColliderManager.IsColliderRegistered(collider.ID.value()))
		{
			// The collider is not registered to this manager.
			return;
		}

		m_ColliderManager.UnregisterAABB(collider.ID.value());
	}


	void MainMenuLayer::OnDetach()
	{
	}

	void MainMenuLayer::OnUpdate()
	{
		debugInput();

		// UI menu input.
		int const prevSelectedButtonIndex = m_SelectedButtonIndex;
		if (INPUT.GetKeyDown(KeyCode::Up) || INPUT.GetKeyDown(KeyCode::W))
		{
			m_SelectedButtonIndex--;
		}
		else if (INPUT.GetKeyDown(KeyCode::Down) || INPUT.GetKeyDown(KeyCode::S))
		{
			m_SelectedButtonIndex++;
		}

		if (INPUT.GetGamepadButtonDown(0, GamepadButton::DPadUp))
		{
			m_SelectedButtonIndex--;
		}
		else if (INPUT.GetGamepadButtonDown(0, GamepadButton::DPadDown))
		{
			m_SelectedButtonIndex++;
		}

		if (m_SelectedButtonIndex < 0)
		{
			m_SelectedButtonIndex = m_MenuButtons.size() - 1;
		}
		else if (m_SelectedButtonIndex == m_MenuButtons.size())
		{
			m_SelectedButtonIndex = 0;
		}

		if (m_SelectedButtonIndex != prevSelectedButtonIndex)
		{
			m_MenuButtons[prevSelectedButtonIndex].SetDeselectAppearance();
			m_MenuButtons[m_SelectedButtonIndex].SetSelectAppearance();

			onSelect(m_SelectedButtonIndex);
		}

		if (INPUT.GetKeyDown(KeyCode::Return))
		{
			onConfirm(m_SelectedButtonIndex);
		}

		if (INPUT.GetGamepadButtonDown(0, GamepadButton::South))
		{
			onConfirm(m_SelectedButtonIndex);
		}
	}

	void MainMenuLayer::onSelect(int index)
	{
		if (index == 0)
		{
			m_SubtitleSprite.Texture = m_LandTheBallSubtitleTexture;
		}
		else if (index == 1)
		{
			m_SubtitleSprite.Texture = m_PongSubtitleTexture;
		}
		else if (index == 2)
		{
			m_SubtitleSprite.Texture = m_ExitSubtitleTexture;
		}
	}

	void MainMenuLayer::onConfirm(int index)
	{
		auto &miniGamesApp = static_cast<MiniGamesApp &>(m_Application);
		if (index == 0)
		{
			miniGamesApp.LoadLandBallLayer();
		}
		else if (index == 1)
		{
			miniGamesApp.LoadPongLayer();
		}
		else if (index == 2)
		{
			DYE_MSG_BOX(SDL_MESSAGEBOX_INFORMATION, "I WANT TO SAY GOODBYE BUT...", "DON'T CLOSE THE GAME DURING THE SHOWCASE :)");
			// miniGamesApp.Shutdown();
		}
	}

	void MainMenuLayer::debugInput()
	{
		if (INPUT.GetKeyDown(KeyCode::F9))
		{
			m_DrawImGui = !m_DrawImGui;
		}

		if (INPUT.GetKeyDown(KeyCode::Escape))
		{
			m_Application.Shutdown();
		}
	}

	void MainMenuLayer::OnFixedUpdate()
	{
	}

	void MainMenuLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(m_MainCamera.GetTransformedProperties());

		// Scroll background texture.
		float const backgroundTilingOffsetChange = TIME.DeltaTime() * m_BackgroundScrollingSpeed;
		m_BackgroundSprite.TilingOffset += glm::vec2 {backgroundTilingOffsetChange, backgroundTilingOffsetChange};
		if (m_BackgroundSprite.TilingOffset.x > 1.0f)
		{
			m_BackgroundSprite.TilingOffset -= glm::vec2 {1.0f, 1.0f};
		}
		renderSprite(m_BackgroundTransform, m_BackgroundSprite);

		// Title logo.
		renderSprite(m_LogoTransform, m_LogoSprite);

		// Buttons.
		for (auto& button : m_MenuButtons)
		{
			renderSprite(button.Transform, button.Sprite);
		}

		// Button Prompt.
		renderSprite(m_ButtonPromptTransform, m_ButtonPromptSprite);

		// Subtitles.
		renderSprite(m_SubtitleTransform, m_SubtitleSprite);

		// Land the ball high score.
		if (m_SelectedButtonIndex == 0)
		{
			m_LandTheBallHighScoreNumber.Render();
			renderSprite(m_HighScoreTextTransform, m_HighScoreTextSprite);
		}
	}

	void MainMenuLayer::renderSprite(MiniGame::Transform &transform, MiniGame::Sprite &sprite)
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

	void MainMenuLayer::OnImGui()
	{
		if (!m_DrawImGui)
		{
			return;
		}

		if (ImGui::Begin("Menu"))
		{
			auto &miniGamesApp = static_cast<MiniGamesApp &>(m_Application);
			ImGui::SameLine();
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
		ImGui::End();

		WindowManager::DrawWindowManagerImGui();

		ImGui::ShowDemoWindow();
	}

	void MainMenuLayer::imguiSprite(const std::string &name, MiniGame::Transform &transform, MiniGame::Sprite &sprite)
	{

	}
}