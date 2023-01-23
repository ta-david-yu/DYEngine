#include "PeepholismLayer.h"

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

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DYE
{
	PeepholismLayer::PeepholismLayer()
	{
	}

	void PeepholismLayer::OnInit()
	{
		RenderCommand::GetInstance().SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		m_PlayerPaddle1.Transform.Position = {-4, 0, 0};
		m_PlayerPaddle1.Collider.Size = {0.75f, 3, 1};

		m_PlayerPaddle2.Transform.Position = {4, 0, 0};
		m_PlayerPaddle2.Collider.Size = {0.75f, 3, 1};

		registerBoxCollider(m_PlayerPaddle1.Transform, m_PlayerPaddle1.Collider);
		registerBoxCollider(m_PlayerPaddle2.Transform, m_PlayerPaddle2.Collider);

		m_MovingObject = std::make_shared<SpriteObject>();
		m_MovingObject->Name = "Moving";
		m_MovingObject->Texture = Texture2D::Create("assets\\Sprite_Pong.png");
		m_MovingObject->Texture->PixelsPerUnit = 32;

		m_BackgroundTileObject = std::make_shared<SpriteObject>();
		m_BackgroundTileObject->Name = "Background";
		m_BackgroundTileObject->Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundTileObject->Texture->PixelsPerUnit = 32;
		m_BackgroundTileObject->Scale = {64.0f, 64.0f, 1};
		m_BackgroundTileObject->Position = {0, 0, -2};

		auto mainWindowPtr = WindowManager::GetMainWindow();
		mainWindowPtr->SetWindowSize(1600, 900);

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


		// Use the same context of the main window for the second window
		m_SecondWindow = WindowManager::CreateWindow(WindowProperty("SecondWindow", 640, 480));
		auto context = mainWindowPtr->GetContext();
		m_SecondWindow->SetContext(context);
		m_SecondWindow->MakeCurrent();
		ContextBase::SetVSyncCountForCurrentContext(0);

		// Set the current context back to the main window.
		mainWindowPtr->MakeCurrent();

		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({5, 0, 0}, {1, 12, 0}));
		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({-5, 2, 0}, {1, 12, 0}));
		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({0, 5, 0}, {12, 1, 0}));
		m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({0, -6, 0}, {12, 3, 0}));
	}

	void PeepholismLayer::registerBoxCollider(Pong::Transform &transform, Pong::BoxCollider &collider)
	{
		if (collider.ID.has_value() && m_ColliderManager.IsColliderRegistered(collider.ID.value()))
		{
			// The collider has already been registered to the manager.
			return;
		}

		collider.ID = m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter(transform.Position, collider.Size));
	}

	void PeepholismLayer::unregisterBoxCollider(Pong::Transform &transform, Pong::BoxCollider &collider)
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

	void PeepholismLayer::updateBoxCollider(Pong::Transform &transform, Pong::BoxCollider &collider)
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

		m_ColliderManager.SetAABB(collider.ID.value(), Math::AABB::CreateFromCenter(transform.Position, collider.Size));
	}

	void PeepholismLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(*m_CameraProperties);

		CameraProperties cameraRenderToSecondWindow {};
		cameraRenderToSecondWindow.Position = {m_MovingObject->Position.x, m_MovingObject->Position.y, 10};
		cameraRenderToSecondWindow.IsOrthographic = true;
		cameraRenderToSecondWindow.OrthographicSize = 10;
		cameraRenderToSecondWindow.TargetType = RenderTargetType::Window;
		cameraRenderToSecondWindow.TargetWindowID = m_SecondWindow->GetWindowID();
		cameraRenderToSecondWindow.UseManualAspectRatio = false;
		cameraRenderToSecondWindow.ViewportValueType = ViewportValueType::RelativeDimension;
		cameraRenderToSecondWindow.Viewport = {0, 0, 1, 1};
		RenderPipelineManager::RegisterCameraForNextRender(cameraRenderToSecondWindow);

		renderSpriteObject(*m_MovingObject);

		renderTiledSpriteObject(*m_BackgroundTileObject, {m_TileOffset, m_TileOffset});
	}

	void PeepholismLayer::renderSpriteObject(SpriteObject& object)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, object.Position);
		modelMatrix = modelMatrix * glm::toMat4(object.Rotation);
		modelMatrix = glm::scale(modelMatrix, object.Scale);

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
			->SubmitSprite(object.Texture, object.Color, modelMatrix);
	}

	void PeepholismLayer::renderTiledSpriteObject(SpriteObject& object, glm::vec2 offset)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, object.Position);
		modelMatrix = modelMatrix * glm::toMat4(object.Rotation);
		modelMatrix = glm::scale(modelMatrix, object.Scale);

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
			->SubmitTiledSprite(object.Texture, {object.Scale.x, object.Scale.y, offset.x, offset.y}, object.Color, modelMatrix);
	}

	void PeepholismLayer::OnEvent(Event& event)
	{

	}

	void PeepholismLayer::OnUpdate()
	{
		m_ColliderManager.DrawGizmos();

		Math::AABB const movingAABB = Math::AABB::CreateFromCenter(m_MovingObject->Position, {0.5f, 0.5f, 0.5f});

		float const circleRadius = 0.25f;
		bool const isMovingOverlapped = !m_ColliderManager.OverlapCircle(m_MovingObject->Position, circleRadius).empty();
		//DebugDraw::Circle(m_MovingObject->Position, circleRadius, {0, 0, 1}, isMovingOverlapped? Color::Red : Color::Yellow);

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
			m_BallVelocity.y += TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Down))
		{
			m_BallVelocity.y -= TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Right))
		{
			m_BallVelocity.x += TIME.DeltaTime() * m_BallSpeed;
		}

		if (INPUT.GetKey(KeyCode::Left))
		{
			m_BallVelocity.x -= TIME.DeltaTime() * m_BallSpeed;
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

		if (INPUT.GetKeyDown(KeyCode::Space))
		{
			m_BallVelocity += glm::vec2 {0, 5};
		}

		auto mainWindowPtr = WindowManager::GetMainWindow();
		if (change)
		{
			m_WindowPosition = glm::round(m_WindowPosition);
			mainWindowPtr->SetWindowPosition(m_WindowPosition.x, m_WindowPosition.y);
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
		}

		if (INPUT.GetKeyDown(KeyCode::F3))
		{
			m_TargetWindowWidth = 1600;
			m_TargetWindowHeight = 900;
		}

		if (INPUT.GetKeyDown(KeyCode::F4))
		{
			m_TargetWindowWidth = 800;
			m_TargetWindowHeight = 640;
		}

		if (INPUT.GetKeyDown(KeyCode::F5))
		{
			m_TargetWindowWidth = 640;
			m_TargetWindowHeight = 640;
		}

		if (INPUT.GetKeyDown(KeyCode::F6))
		{
			m_TargetWindowWidth = 320;
			m_TargetWindowHeight = 320;
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

		if (INPUT.GetKeyDown(KeyCode::Escape))
		{
			Application::GetRegisteredApplications()[0]->Shutdown();
			return;
		}

		if (INPUT.IsGamepadConnected(0))
		{
			float const horizontal = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickHorizontal);
			float const vertical = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickVertical);

			glm::vec3 const axis = {0, -vertical, 0};

			if (glm::length2(axis) > 0.01f)
			{
				m_PlayerPaddle1.MovementInputBuffer = axis;
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
			float const newWidth = Math::Lerp(currWidth, m_TargetWindowWidth, 1.0f - glm::pow(0.5f, TIME.DeltaTime() / 0.1f));
			float const newHeight = Math::Lerp(currHeight, m_TargetWindowHeight, 1.0f - glm::pow(0.5f, TIME.DeltaTime() / 0.1f));

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

	void PeepholismLayer::OnFixedUpdate()
	{
		m_FixedUpdateCounter++;

		float const ballRadius = 0.25f;

		float const inputSqr = glm::length2(m_PlayerPaddle1.MovementInputBuffer);
		glm::vec3 paddleVelocityPerSecond = glm::vec3 {0, 0, 0};
		if (inputSqr > glm::epsilon<float>())
		{
			// Calculate the paddleVelocity
			float const magnitude = glm::length(m_PlayerPaddle1.MovementInputBuffer);
			glm::vec3 const direction = glm::normalize(glm::vec3{m_PlayerPaddle1.MovementInputBuffer, 0});
			paddleVelocityPerSecond = m_PlayerPaddle1.Speed * direction * magnitude;
			glm::vec3 const paddleVelocity = paddleVelocityPerSecond * (float) TIME.FixedDeltaTime();

			// Push the ball if there is an overlap.
			Math::DynamicTestResult2D result2D;
			bool const intersectBall = Math::MovingCircleAABBIntersect(m_MovingObject->Position, ballRadius, -paddleVelocity, m_PlayerPaddle1.GetAABB(), result2D);
			if (intersectBall)
			{
				glm::vec2 const normal = glm::normalize(result2D.HitNormal);
				auto ballPaddleDot = glm::dot(paddleVelocity, glm::vec3 {m_BallVelocity, 0});

				if (ballPaddleDot < 0.0f)
				{
					// If the ball is moving in the opposite direction of the paddle,
					// bounce (deflect) the ball into reflected direction.
					m_BallVelocity = m_BallVelocity - 2 * glm::dot(normal, m_BallVelocity) * normal;
					m_BallVelocity += glm::vec2 {paddleVelocityPerSecond.x, paddleVelocityPerSecond.y};
				}

				// Move the ball away from the paddle to avoid tunneling
				m_MovingObject->Position += paddleVelocity;
			}

			// Update collider info stored in the collider manager.
			updateBoxCollider(m_PlayerPaddle1.Transform, m_PlayerPaddle1.Collider);

			// Move the paddle.
			m_PlayerPaddle1.Transform.Position += paddleVelocity;
			m_PlayerPaddle1.MovementInputBuffer = {0, 0};
		}

		auto const timeStep = (float) TIME.FixedDeltaTime();
		glm::vec2 const positionChange = timeStep * m_BallVelocity;
		auto const& hits = m_ColliderManager.CircleCastAll(m_MovingObject->Position, 0.25f, positionChange);
		if (!hits.empty())
		{
			// Collide with a box!
			auto const& hit = hits[0];
			glm::vec2 const normal = glm::normalize(hit.Normal);

			float const minimumTravelTimeAfterReflected = 0.001f;
			float const reflectedTravelTime = glm::max(minimumTravelTimeAfterReflected, timeStep - hit.Time);
			m_BallVelocity = m_BallVelocity - 2 * glm::dot(normal, m_BallVelocity) * normal;
			m_MovingObject->Position = glm::vec3(hit.Centroid + reflectedTravelTime * m_BallVelocity, 0);

			if (hit.ColliderID == m_PlayerPaddle1.Collider.ID)
			{
				// The ball hits a paddle!
				// Adjust velocity based on the paddle's velocity.
				m_BallVelocity += glm::vec2 {paddleVelocityPerSecond.x, paddleVelocityPerSecond.y};
			}
		}
		else
		{
			m_MovingObject->Position += glm::vec3(positionChange, 0);
		}
	}

	void PeepholismLayer::OnImGui()
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

			if (ImGui::Button("Quit App"))
			{
				for (auto const app : Application::GetRegisteredApplications())
				{
					app->Shutdown();
				}
			}

			ImGui::SameLine();
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
			imguiSpriteObject(*m_BackgroundTileObject);
		}

		ImGui::End();

		m_ColliderManager.DrawImGui();
		INPUT.DrawAllRegisteredDeviceDescriptorsImGui();
		INPUT.DrawAllConnectedDeviceDescriptorsImGui();
	}

	void PeepholismLayer::imguiSpriteObject(SpriteObject &object)
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