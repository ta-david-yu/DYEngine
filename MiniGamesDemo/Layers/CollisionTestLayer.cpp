#include "CollisionTestLayer.h"

#include "MiniGamesDemo/MiniGamesApp.h"

#include "Core/Application.h"
#include "Util/Logger.h"
#include "Util/Macro.h"
#include "Util/Time.h"

#include "Math/Color.h"
#include "Math/Math.h"

#include "ImGui/ImGuiUtil.h"

#include "Input/InputManager.h"
#include "Screen.h"
#include "Math/PrimitiveTest.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"

#include "Graphics/WindowBase.h"
#include "Graphics/WindowManager.h"
#include "Graphics/ContextBase.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Texture.h"
#include "Graphics/CameraProperties.h"
#include "Graphics/Material.h"
#include "Graphics/DebugDraw.h"

#include "Event/ApplicationEvent.h"

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"

namespace DYE
{
	CollisionTestLayer::CollisionTestLayer(Application& application) : m_Application(application)
    {
    }

	void CollisionTestLayer::OnAttach()
	{
		RenderCommand::GetInstance().SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		m_OriginObject = std::make_shared<SpriteObject>();
		m_OriginObject->Name = "Origin";
		m_OriginObject->Texture = Texture2D::Create("assets\\Sprite_Pong.png");
		m_OriginObject->Texture->PixelsPerUnit = 32;

		m_AverageObject = std::make_shared<SpriteObject>();
		m_AverageObject->Name = "Average";
		m_AverageObject->Texture = m_OriginObject->Texture;

		m_MovingObject = std::make_shared<SpriteObject>();
		m_MovingObject->Name = "Moving";
		m_MovingObject->Texture = m_OriginObject->Texture;

		m_BackgroundTileObject = std::make_shared<SpriteObject>();
		m_BackgroundTileObject->Name = "Background";
		m_BackgroundTileObject->Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundTileObject->Texture->PixelsPerUnit = 32;
		m_BackgroundTileObject->Scale = {16.0f, 10.0f, 1};
		m_BackgroundTileObject->Position = {0, 0, -2};

		auto mainWindowPtr = WindowManager::GetMainWindow();
		mainWindowPtr->SetSize(1600, 900);

		m_CameraProperties = std::make_shared<CameraProperties>();
		m_CameraProperties->Position = glm::vec3 {0, 0, 10};
		m_CameraProperties->IsOrthographic = true;
		m_CameraProperties->OrthographicSize = 10;
		m_CameraProperties->TargetType = RenderTargetType::Window;
		m_CameraProperties->TargetWindowID = mainWindowPtr->GetWindowID();
		m_CameraProperties->UseManualAspectRatio = false;
		m_CameraProperties->ManualAspectRatio = (float) 1600 / 900;
		m_CameraProperties->ViewportValueType = ViewportValueType::RelativeDimension;
		m_CameraProperties->Viewport = { 0, 0, 1, 1 };

		m_WindowPosition = mainWindowPtr->GetPosition();

		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({2, 0, 0}, {1, 1, 0}));
		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({-2, 0, 0}, {1, 1, 0}));
		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({0, 2, 0}, {1, 1, 0}));
		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({0, -3, 0}, {2, 3, 0}));
	}

	void CollisionTestLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(*m_CameraProperties);

		renderSpriteObject(*m_OriginObject);
		renderSpriteObject(*m_MovingObject);
		renderSpriteObject(*m_AverageObject);

		renderTiledSpriteObject(*m_BackgroundTileObject, {m_TileOffset, m_TileOffset});
	}

	void CollisionTestLayer::renderSpriteObject(SpriteObject& object)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, object.Position);
		modelMatrix = modelMatrix * glm::toMat4(object.Rotation);
		modelMatrix = glm::scale(modelMatrix, object.Scale);

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
		    ->SubmitSprite(object.Texture, object.Color, modelMatrix);
	}

	void CollisionTestLayer::renderTiledSpriteObject(SpriteObject& object, glm::vec2 offset)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, object.Position);
		modelMatrix = modelMatrix * glm::toMat4(object.Rotation);
		modelMatrix = glm::scale(modelMatrix, object.Scale);

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
		    ->SubmitTiledSprite(object.Texture, {object.Scale.x, object.Scale.y, offset.x, offset.y}, object.Color, modelMatrix);
	}

    void CollisionTestLayer::OnUpdate()
    {
		m_ColliderManager.DrawGizmos();

		Math::AABB const movingAABB = Math::AABB::CreateFromCenter(m_MovingObject->Position, {0.5f, 0.5f, 0.5f});
		Math::AABB const averageAABB = Math::AABB::CreateFromCenter(m_AverageObject->Position, {0.5f, 0.5f, 0.5f});

		float const circleRadius = 0.25f;
		bool const isMovingOverlapped = !m_ColliderManager.OverlapCircle(m_MovingObject->Position, circleRadius).empty();
		bool const isAverageOverlapped = !m_ColliderManager.OverlapCircle(m_AverageObject->Position, circleRadius).empty();

		DebugDraw::Circle(m_MovingObject->Position, circleRadius, {0, 0, 1}, isMovingOverlapped? Color::Red : Color::Yellow);
		DebugDraw::Circle(m_AverageObject->Position, circleRadius, {0, 0, 1}, isAverageOverlapped? Color::Red : Color::Yellow);

		glm::vec2 const rayStart = m_AverageObject->Position;
		glm::vec2 const rayEnd = m_MovingObject->Position;

		/*
		auto hits = m_ColliderManager.RaycastAll(rayStart, rayEnd);
		for (auto& hit : hits)
		{
			glm::vec3 const hitPoint3D = {hit.Point.x, hit.Point.y, 0};
			glm::vec3 const hitNormal3D = {hit.Normal.x, hit.Normal.y, 0};
			DebugDraw::Line(hitPoint3D, hitPoint3D + hitNormal3D, Color::Yellow);
			DebugDraw::Circle(hitPoint3D, 0.05f, {0, 0, 1}, Color::Red);
		}*/

		auto hits = m_ColliderManager.CircleCastAll(rayStart, circleRadius, rayEnd - rayStart);
		for (auto& hit : hits)
		{
			glm::vec3 const hitPoint3D = {hit.Point.x, hit.Point.y, 0};
			glm::vec3 const hitNormal3D = {hit.Normal.x, hit.Normal.y, 0};
			DebugDraw::Line(hitPoint3D, hitPoint3D + hitNormal3D, Color::Yellow);
			DebugDraw::Circle(hitPoint3D, 0.05f, {0, 0, 1}, Color::Red);
			DebugDraw::Circle({hit.Centroid.x, hit.Centroid.y, 0}, circleRadius, {0, 0, 1}, Color::Red);
		}

		bool const hasIntersect = !hits.empty();
		DebugDraw::Line(m_MovingObject->Position, m_AverageObject->Position, hasIntersect? Color::Red : Color::Yellow);

		// Scroll tiled offset
		m_TileOffset += TIME.DeltaTime() * 0.5f;
		if (m_TileOffset > 1.0f)
		{
			m_TileOffset -= 1.0f;
		}

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
			m_MovingObject->Position.y += TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Down))
		{
			m_MovingObject->Position.y -= TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Right))
		{
			m_MovingObject->Position.x += TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Left))
		{
			m_MovingObject->Position.x -= TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetMouseButtonDown(MouseButton::Left))
		{
			m_MovingObject->Color = Color::Yellow;
		}

		if (INPUT.GetMouseButtonDown(MouseButton::Right))
		{
			m_MovingObject->Color = Color::Blue;
		}

		if (INPUT.GetMouseButtonDown(MouseButton::Middle))
		{
			m_MovingObject->Color = Color::Red;
		}

		m_AverageObject->Position = (m_MovingObject->Position + m_OriginObject->Position) * 0.5f;

		bool change = false;
		if (INPUT.GetKey(KeyCode::W))
		{
			m_WindowPosition.y -= TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}
		if (INPUT.GetKey(KeyCode::S))
		{
			m_WindowPosition.y += TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}
		if (INPUT.GetKey(KeyCode::D))
		{
			m_WindowPosition.x += TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}
		if (INPUT.GetKey(KeyCode::A))
		{
			m_WindowPosition.x -= TIME.DeltaTime() * m_WindowPixelChangePerSecond;
			change = true;
		}

		auto mainWindowPtr = WindowManager::GetMainWindow();
		if (change)
		{
			m_WindowPosition = glm::round(m_WindowPosition);
			mainWindowPtr->SetPosition(m_WindowPosition.x, m_WindowPosition.y);
		}

		if (INPUT.GetKeyDown(KeyCode::F1))
		{
			mainWindowPtr->CenterWindow();
			m_WindowPosition = mainWindowPtr->GetPosition();
		}

		if (INPUT.GetKeyDown(KeyCode::F2))
		{
			m_TargetWindowWidth = 1920;
			m_TargetWindowHeight = 1080;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(1920, 1080);
		}

		if (INPUT.GetKeyDown(KeyCode::F3))
		{
			m_TargetWindowWidth = 1600;
			m_TargetWindowHeight = 900;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(1600, 900);
		}

		if (INPUT.GetKeyDown(KeyCode::F4))
		{
			m_TargetWindowWidth = 800;
			m_TargetWindowHeight = 640;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(800, 640);
		}

		if (INPUT.GetKeyDown(KeyCode::F5))
		{
			m_TargetWindowWidth = 640;
			m_TargetWindowHeight = 640;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(640, 640);
		}

		if (INPUT.GetKeyDown(KeyCode::F6))
		{
			m_TargetWindowWidth = 320;
			m_TargetWindowHeight = 320;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(320, 320);
		}

		if (INPUT.GetKeyDown(KeyCode::F7))
		{
			m_TargetWindowWidth = 280;
			m_TargetWindowHeight = 280;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(320, 320);
		}

		if (INPUT.GetKeyDown(KeyCode::F8))
		{
			m_TargetWindowWidth = 240;
			m_TargetWindowHeight = 240;
			//m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(320, 320);
		}

		if (INPUT.IsGamepadConnected(0))
		{
			float const horizontal = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickHorizontal);
			float const vertical = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickVertical);

			glm::vec3 const axis = {horizontal, vertical, 0};

			if (glm::length2(axis) > 0.01f)
			{
				float const magnitude = glm::length(axis);
				glm::vec3 const direction = glm::normalize(axis);
				m_MovingObject->Position += direction * (magnitude * (float) TIME.DeltaTime());
			}
		}

		int currWidth = mainWindowPtr->GetWidth();
		int currHeight = mainWindowPtr->GetHeight();

		int const widthDiff = m_TargetWindowWidth - currWidth;
		int const heightDiff = m_TargetWindowHeight - currHeight;
		if (glm::abs(widthDiff) > 0 || glm::abs(heightDiff) > 0)
		{
			// TODO: Fixed precision issue because window size is int, but animation is using float!
			//  We prolly want to use fixed animation instead (tween animation)
			float newWidth = Math::Lerp(currWidth, m_TargetWindowWidth, 1.0f - glm::pow(0.5f, TIME.DeltaTime() / 0.1f));
			float newHeight = Math::Lerp(currHeight, m_TargetWindowHeight, 1.0f - glm::pow(0.5f, TIME.DeltaTime() / 0.1f));

			// Make the speed even, so SetWindowSizeUsingWindowCenterAsAnchor can properly derive precise integer position
			int widthSpeed = glm::abs(newWidth - currWidth);
			if (widthSpeed % 2 == 1) widthSpeed += 1;

			int heightSpeed = glm::abs(newHeight - currHeight);
			if (heightSpeed % 2 == 1) heightSpeed += 1;

			currWidth += glm::sign(widthDiff) * std::min((int) widthSpeed, glm::abs(widthDiff));
			currHeight += glm::sign(heightDiff) * std::min((int) heightSpeed, glm::abs(heightDiff));
			m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(currWidth, currHeight);

			if (glm::abs(m_TargetWindowWidth - currWidth) < 5 && glm::abs(m_TargetWindowHeight - currHeight) < 5)
			{
				m_WindowPosition = mainWindowPtr->SetWindowSizeUsingWindowCenterAsAnchor(m_TargetWindowWidth, m_TargetWindowHeight);
			}
		}

		glm::vec2 const windowPos = mainWindowPtr->GetPosition();
		auto windowWidth = mainWindowPtr->GetWidth();
		auto windowHeight = mainWindowPtr->GetHeight();

		glm::vec2 normalizedWindowPos = windowPos;
		normalizedWindowPos.x += windowWidth * 0.5f;
		normalizedWindowPos.y += windowHeight * 0.5f;

		int const mainDisplayIndex = mainWindowPtr->GetDisplayIndex();
		std::optional<DisplayMode> const displayMode = SCREEN.GetDisplayMode(mainDisplayIndex);
		float const screenWidth = displayMode->Width;
		float const screenHeight = displayMode->Height;

		normalizedWindowPos.y = screenHeight - normalizedWindowPos.y;

		float const scalar = 0.5f; //* (320.0f / windowHeight);
		m_CameraProperties->Position.x = ((normalizedWindowPos.x - screenWidth * 0.5f) / 32.0f) * scalar;
		m_CameraProperties->Position.y = ((normalizedWindowPos.y - screenHeight * 0.5f) / 32.0f) * scalar;

		float const sizeScalar = 1.0f * (320.0f / windowHeight);
    	m_CameraProperties->OrthographicSize = 5 / sizeScalar;
	}

    void CollisionTestLayer::OnFixedUpdate()
    {
        m_FixedUpdateCounter++;
        //SDL_Log("FixedDeltaTime - %f", TIME.FixedDeltaTime());
    }

    void CollisionTestLayer::OnImGui()
    {
        // get the window size as a base for calculating widgets geometry
		auto mainWindowPtr = WindowManager::GetMainWindow();
        int windowWidth = mainWindowPtr->GetWidth();
		int windowHeight = mainWindowPtr->GetHeight();
		int controls_width = windowWidth;
        // make controls widget width to be 1/3 of the main window width
        if ((controls_width * 0.33f) < 300) { controls_width = 300; }

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
                ImVec2(static_cast<float>(controls_width), static_cast<float>(windowHeight * 0.5f)),
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
			ImGui::Text("expected FPS: [%d]", TIME.FixedFramePerSecond());
			ImGui::Text("DeltaTime: [%f]", TIME.DeltaTime());
			ImGui::Text("FixedDeltaTime: [%f]", TIME.FixedDeltaTime());
			ImGui::Text("FixedUpdateFrameCounter: [%d]", m_FixedUpdateCounter);

			int const mainDisplayIndex = mainWindowPtr->GetDisplayIndex();
			ImGui::Text("MainWindowDisplayIndex = %d", mainDisplayIndex);
			std::optional<DisplayMode> const displayMode = SCREEN.GetDisplayMode(mainDisplayIndex);
			if (displayMode.has_value())
			{
				ImGui::Text("Display %d Info = (w=%d, h=%d, r=%d)", mainDisplayIndex, displayMode->Width, displayMode->Height, displayMode->RefreshRate);
			}

			ImGui::Text("Mouse Position = %d, %d", INPUT.GetGlobalMousePosition().x, INPUT.GetGlobalMousePosition().y);
			ImGui::Text("Mouse Delta = %d, %d", INPUT.GetGlobalMouseDelta().x, INPUT.GetGlobalMouseDelta().y);

			static bool isMainWindowBordered = true;
			if (ImGui::Button("Toggle Window Bordered"))
			{
				isMainWindowBordered = !isMainWindowBordered;
				mainWindowPtr->SetBorderedIfWindowed(isMainWindowBordered);
			}

			ImGui::SameLine();
			if (ImGui::Button("Center Window"))
			{
				mainWindowPtr->CenterWindow();
				m_WindowPosition = mainWindowPtr->GetPosition();
			}

			auto& miniGamesApp = static_cast<MiniGamesApp&>(m_Application);
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

			if (ImGui::Button("Line Mode"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			ImGui::SameLine();
			if (ImGui::Button("Fill Mode"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			ImGui::Separator();
			ImGuiUtil::DrawCameraPropertiesControl("Camera Properties", *m_CameraProperties);

			ImGui::Separator();
			ImGuiUtil::DrawFloatControl("Camera Speed", m_WindowPixelChangePerSecond, 300);

			ImGui::Separator();
			imguiSpriteObject(*m_MovingObject);

			ImGui::Separator();
			imguiSpriteObject(*m_AverageObject);

			ImGui::Separator();
			imguiSpriteObject(*m_OriginObject);

			ImGui::Separator();
			imguiSpriteObject(*m_BackgroundTileObject);
		}

        ImGui::End();

		m_ColliderManager.DrawImGui();
		INPUT.DrawAllRegisteredDeviceDescriptorsImGui();
		INPUT.DrawAllConnectedDeviceDescriptorsImGui();
    }

	void CollisionTestLayer::imguiSpriteObject(SpriteObject &object)
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