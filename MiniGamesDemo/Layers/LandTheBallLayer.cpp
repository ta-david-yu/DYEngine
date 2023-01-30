#include "Layers/LandTheBallLayer.h"

#include "MiniGamesApp.h"

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
	LandTheBallLayer::LandTheBallLayer(Application &application) : m_Application(application)
	{

	}

	void LandTheBallLayer::OnAttach()
	{
		// Create background object.
		m_BackgroundSprite.Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundSprite.Texture->PixelsPerUnit = 32;
		m_BackgroundSprite.IsTiled = true;
		m_BackgroundTransform.Scale = {64.0f, 64.0f, 1};
		m_BackgroundTransform.Position = {0, 0, -2};

		// Number
		m_Number.Transform.Scale = {2, 2, 1};
		m_Number.Transform.Position = {0, 1, 0};
		m_Number.DigitDistanceOffset = 0.5f;
		m_Number.LoadTexture();
		m_Number.SetValue(1598);

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
	}

	void LandTheBallLayer::registerBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
	{
		if (collider.ID.has_value() && m_ColliderManager.IsColliderRegistered(collider.ID.value()))
		{
			// The collider has already been registered to the manager.
			return;
		}

		collider.ID = m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter(transform.Position, collider.Size));
	}

	void LandTheBallLayer::unregisterBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
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


	void LandTheBallLayer::OnDetach()
	{
	}

	void LandTheBallLayer::OnUpdate()
	{
		debugInput();
	}

	void LandTheBallLayer::debugInput()
	{
		if (INPUT.GetKeyDown(KeyCode::F9))
		{
			m_DrawImGui = !m_DrawImGui;
		}

		auto value = m_Number.GetValue();
		if (INPUT.GetKey(KeyCode::Numpad3))
		{
			value++;
			m_Number.SetValue(value);
		}
		if (INPUT.GetKey(KeyCode::Numpad1))
		{
			if (value > 0)
			{
				value--;
			}
			m_Number.SetValue(value);
		}

		if (INPUT.GetKeyDown(KeyCode::Escape))
		{
			m_Application.Shutdown();
		}
	}

	void LandTheBallLayer::OnFixedUpdate()
	{
	}

	void LandTheBallLayer::OnRender()
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

		m_Number.Render();
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

			auto value = m_Number.GetValue();
			if (ImGui::Button("+"))
			{
				value++;
				m_Number.SetValue(value);
			}

			if (ImGui::Button("-"))
			{
				if (value > 0)
				{
					value--;
				}
				m_Number.SetValue(value);
			}
		}
		ImGui::End();
	}
}