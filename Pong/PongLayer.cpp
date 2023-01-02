#include "Base.h"

#include "PongLayer.h"
#include "WindowBase.h"
#include "WindowManager.h"
#include "ContextBase.h"
#include "Logger.h"

#include "Util/Time.h"
#include "Util/ImGuiUtil.h"

#include "Input/InputManager.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Texture.h"
#include "Graphics/CameraProperties.h"
#include "Graphics/Material.h"
#include "Event/ApplicationEvent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DYE
{
	PongLayer::PongLayer()
    {
    }

	void PongLayer::OnInit()
	{
		RenderCommand::GetInstance().SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		m_ProfileObject = std::make_shared<SpriteObject>();
		m_ProfileObject->Name = "Profile";
		m_ProfileObject->Texture = Texture2D::Create("assets\\Sprite_Pong.png");
		m_ProfileObject->Texture->PixelsPerUnit = 32;

		m_WhiteObject = std::make_shared<SpriteObject>();
		m_WhiteObject->Name = "White";
		m_WhiteObject->Texture = Texture2D::Create("assets\\Sprite_Pong.png");
		m_WhiteObject->Texture->PixelsPerUnit = 32;

		auto mainWindowPtr = WindowManager::GetMainWindow();

		m_CameraProperties = std::make_shared<CameraProperties>();
		m_CameraProperties->Position = glm::vec3 {0, 0, 10};
		m_CameraProperties->IsOrthographic = true;
		m_CameraProperties->OrthographicSize = 10;

		m_CameraProperties->TargetType = RenderTargetType::Window;
		m_CameraProperties->TargetWindowID = mainWindowPtr->GetWindowID();

		m_CameraProperties->UseManualAspectRatio = true;
		m_CameraProperties->ManualAspectRatio = (float) mainWindowPtr->GetWidth() / (float) mainWindowPtr->GetHeight();

		/*
		m_CameraProperties->ViewportValueType = ViewportValueType::AbsoluteDimension;
		m_CameraProperties->Viewport = { 0, 0, (float) mainWindowPtr->GetWidth(), (float) mainWindowPtr->GetHeight() };*/

		m_CameraProperties->ViewportValueType = ViewportValueType::RelativeDimension;
		m_CameraProperties->Viewport = { 0, 0, 1, 1 };

		m_WindowWidth = mainWindowPtr->GetWidth();
		m_WindowHeight = mainWindowPtr->GetHeight();

		m_SecondWindow = WindowManager::CreateWindow(WindowProperty("Second Window"));

		// Share the context with the main window
		//SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		//auto context = ContextBase::Create(m_SecondWindow);
		//m_SecondWindow->SetContext(context);

		// Use the same context of the main window for the second window
		auto context = mainWindowPtr->GetContext();
		m_SecondWindow->SetContext(context);

		// Set the current context back to the main window.
		context->MakeCurrentForWindow(*mainWindowPtr);
	}

	void PongLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(*m_CameraProperties);

		CameraProperties cameraRenderToSecondWindow = *m_CameraProperties;
		cameraRenderToSecondWindow.UseManualAspectRatio = false;
		cameraRenderToSecondWindow.Viewport = {0.5f, 0.5f, 0.5f, 0.5f};
		cameraRenderToSecondWindow.TargetWindowID = m_SecondWindow->GetWindowID();
		RenderPipelineManager::RegisterCameraForNextRender(cameraRenderToSecondWindow);

		renderMaterialObject(*m_ProfileObject);
		renderMaterialObject(*m_WhiteObject);
	}

	void PongLayer::renderMaterialObject(SpriteObject& object)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, object.Position);
		modelMatrix = modelMatrix * glm::toMat4(object.Rotation);
		modelMatrix = glm::scale(modelMatrix, object.Scale);

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()->SubmitSprite(object.Texture, object.Color, modelMatrix);
	}

    void PongLayer::OnEvent(Event& event)
    {
        auto eventType = event.GetEventType();

		if (eventType == EventType::WindowSizeChange)
		{
			auto windowSizeChangeEvent = static_cast<const WindowManualResizeEvent&>(event);
			//m_CameraProperties->OrthographicSize = 10 * (windowSizeChangeEvent.GetWidth() / 1600.0f);
			//m_CameraProperties->ManualAspectRatio = (float) windowSizeChangeEvent.GetWidth() / (float) windowSizeChangeEvent.GetHeight();
		}
        else if (eventType == EventType::KeyDown)
        {
            auto keyEvent = static_cast<KeyDownEvent&>(event);
            //DYE_ASSERT(keyEvent.GetKeyCode() != KeyCode::Space);
            //DYE_ASSERT_RELEASE(keyEvent.GetKeyCode() != KeyCode::Right);
            DYE_LOG("Sandbox, KeyDown - %s", GetKeyName(keyEvent.GetKeyCode()).c_str());
        }
        else if (eventType == EventType::KeyUp)
        {
            auto keyEvent = static_cast<KeyUpEvent&>(event);
            //DYE_LOG("Sandbox, KeyUp - %d", keyEvent.GetKeyCode());
        }
    }

    void PongLayer::OnUpdate()
    {
        // FPS
        m_FramesCounter++;
        m_FpsAccumulator += TIME.DeltaTime();
        if (m_FpsAccumulator >= 0.25)
        {
            double const fps = m_FramesCounter / m_FpsAccumulator;
            //SDL_Log("Delta FPS: %f", fps);

            m_FramesCounter = 0;
            m_FpsAccumulator = 0;
        }

		if (INPUT.GetKey(KeyCode::Up))
		{
			m_WhiteObject->Position.y += TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Down))
		{
			m_WhiteObject->Position.y -= TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Right))
		{
			m_WhiteObject->Position.x += TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Left))
		{
			m_WhiteObject->Position.x -= TIME.DeltaTime() * m_BallSpeed;
		}

		bool change = false;
		if (INPUT.GetKey(KeyCode::W))
		{
			m_WindowHeight -= TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}
		if (INPUT.GetKey(KeyCode::S))
		{
			m_WindowHeight += TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}
		if (INPUT.GetKey(KeyCode::D))
		{
			m_WindowWidth += TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}
		if (INPUT.GetKey(KeyCode::A))
		{
			m_WindowWidth -= TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}

		auto mainWindowPtr = WindowManager::GetMainWindow();
		if (change)
		{
			mainWindowPtr->SetWindowSize(m_WindowWidth, m_WindowHeight);
		}


		if (INPUT.GetKey(KeyCode::Space))
		{
			m_KeyCounter++;
		}

		if (INPUT.GetKeyDown(KeyCode::Space))
		{
			m_KeyDownCounter++;
		}

		if (INPUT.GetKeyUp(KeyCode::Space))
		{
			m_KeyUpCounter++;
		}

		if (INPUT.GetKeyDown(KeyCode::F1))
		{
			mainWindowPtr->SetWindowSize(1920, 1080);
		}

		if (INPUT.GetKeyDown(KeyCode::F2))
		{
			mainWindowPtr->SetWindowSize(1600, 900);
		}
    }

    void PongLayer::OnFixedUpdate()
    {
        m_FixedUpdateCounter++;
        //SDL_Log("FixedDeltaTime - %f", TIME.FixedDeltaTime());
    }

    void PongLayer::OnImGui()
    {
		//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        static int counter = 0;
        // get the window size as a base for calculating widgets geometry
        int sdl_width = 0, sdl_height = 0, controls_width = 0;
		auto mainWindowPtr = WindowManager::GetMainWindow();
        SDL_GetWindowSize(mainWindowPtr->GetTypedNativeWindowPtr<SDL_Window>(), &sdl_width, &sdl_height);
        controls_width = sdl_width;
        // make controls widget width to be 1/3 of the main window width
        if ((controls_width /= 3) < 300) { controls_width = 300; }

        // position the controls widget in the top-right corner with some margin
        ImGui::SetNextWindowPos
			(
				ImVec2(10, 10),
				ImGuiCond_FirstUseEver
			);
        // here we set the calculated width and also make the height to be
        // the half height of the main window
        ImGui::SetNextWindowSize
			(
                ImVec2(static_cast<float>(controls_width), static_cast<float>(sdl_height * 0.5f)),
                ImGuiCond_FirstUseEver
			);

        // create a window and append into it
		//ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        if (ImGui::Begin("Controls", &m_IsControlWindowOpen))
		{
			ImGui::Dummy(ImVec2(0.0f, 1.0f));
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
			ImGui::Text("%s", SDL_GetPlatform());
			ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
			ImGui::Text("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);

			// buttons and most other widgets return true when clicked/edited/activated
			if (ImGui::Button("Counter button"))
			{
				SDL_Log("Counter Button Clicked");
				counter++;
			}

			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("expected FPS: [%d]", TIME.FixedFramePerSecond());
			ImGui::Text("DeltaTime: [%f]", TIME.DeltaTime());
			ImGui::Text("FixedDeltaTime: [%f]", TIME.FixedDeltaTime());
			ImGui::Text("FixedUpdateFrameCounter: [%d]", m_FixedUpdateCounter);

			if (ImGui::Button("Wireframe Rendering Mode"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			ImGui::SameLine();
			if (ImGui::Button("Normal Rendering Mode"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			ImGui::Text("Space Key: {%d}, Down: {%d}, Up: {%d}", m_KeyCounter, m_KeyDownCounter, m_KeyUpCounter);

			ImGui::Separator();
			ImGuiUtil::DrawCameraPropertiesControl("Camera Properties", *m_CameraProperties);

			ImGui::Separator();
			ImGuiUtil::DrawFloatControl("Camera Speed", m_BallSpeed, 1.0f);

			ImGui::Separator();
			imguiMaterialObject(*m_ProfileObject);

			ImGui::Separator();
			imguiMaterialObject(*m_WhiteObject);
		}

        ImGui::End();

		ImGui::ShowDemoWindow();
    }

	void PongLayer::imguiMaterialObject(SpriteObject &object)
	{
		ImGui::PushID(object.Name.c_str());

		ImGuiUtil::DrawVec3Control("Position##" + object.Name, object.Position);
		ImGuiUtil::DrawVec3Control("Scale##" + object.Name, object.Scale);

		glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(object.Rotation);
		rotationInEulerAnglesDegree += glm::vec3(0.f);
		rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
		if (ImGuiUtil::DrawVec3Control("Rotation##" + object.Name, rotationInEulerAnglesDegree))
		{
			rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
			object.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};
		}

		ImGuiUtil::DrawColor4Control("_Color", object.Color);

		ImGui::PopID();
	}
}