#include "PongLayer.h"

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
	PongLayer::PongLayer()
	{
	}

	void PongLayer::OnInit()
	{
		RenderCommand::GetInstance().SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		// Create ball objects.
		m_Ball.Transform.Position = {0, 0, 0};
		m_Ball.Collider.Radius = 0.25f;
		m_Ball.Velocity.Value = {5.0f, -0.5f};
		m_Ball.LaunchBaseSpeed = 7;
		m_Ball.Sprite.Texture = Texture2D::Create("assets\\Sprite_Pong.png");
		m_Ball.Sprite.Texture->PixelsPerUnit = 32;
		m_Ball.Sprite.Color = Color::White;

		// Create player objects.
		float const mainPaddleWidth = 0.5f;
		float const paddleAttachOffsetX = m_Ball.Collider.Radius + mainPaddleWidth * 0.5f + 0.01f; // Add 0.01f to avoid floating point error

		MiniGame::PongPlayer player1 {};
		player1.Settings.ID = 0;
		player1.Settings.MainPaddleLocation = {-10, 0, 0};
		player1.Settings.MainPaddleAttachOffset = {paddleAttachOffsetX, 0};
		player1.Settings.HomebaseCenter = {-14, 0, 0};	// TODO:
		player1.Settings.HomebaseSize = {2, 16, 1}; 	// TODO:

		player1.State.Health = 5;

		MiniGame::PongPlayer player2 {};
		player2.Settings.ID = 1;
		player2.Settings.MainPaddleLocation = {10, 0, 0};
		player2.Settings.MainPaddleAttachOffset = {-paddleAttachOffsetX, 0};
		player2.Settings.HomebaseCenter = {14, 0, 0};	// TODO:
		player2.Settings.HomebaseSize = {2, 16, 1}; 	// TODO:

		player2.State.Health = 5;

		m_Players.emplace_back(player1);
		m_Players.emplace_back(player2);

		// Create paddle objects.
		for (auto const& player : m_Players)
		{
			MiniGame::PlayerPaddle paddle;
			paddle.PlayerID = player.Settings.ID;
			paddle.Transform.Position = player.Settings.MainPaddleLocation;
			paddle.Collider.Size = {mainPaddleWidth, 3, 1};

			registerBoxCollider(paddle.Transform, paddle.Collider);
			m_PlayerPaddles.emplace_back(paddle);
		}

		// Create goal areas.
		for (auto const& player : m_Players)
		{
			MiniGame::PongHomebase homebase;
			homebase.PlayerID = player.Settings.ID;
			homebase.Transform.Position = player.Settings.HomebaseCenter;
			homebase.Collider.Size = player.Settings.HomebaseSize;

			m_Homebases.emplace_back(homebase);
		}

		// Create wall objects.
		MiniGame::Wall leftUpperWall; leftUpperWall.Transform.Position = {12, 8, 0}; leftUpperWall.Collider.Size = {1, 8, 0};
		MiniGame::Wall rightUpperWall; rightUpperWall.Transform.Position = {-12, 8, 0}; rightUpperWall.Collider.Size = {1, 8, 0};
		MiniGame::Wall leftLowerWall; leftLowerWall.Transform.Position = {12, -8, 0}; leftLowerWall.Collider.Size = {1, 8, 0};
		MiniGame::Wall rightLowerWall; rightLowerWall.Transform.Position = {-12, -8, 0}; rightLowerWall.Collider.Size = {1, 8, 0};

		MiniGame::Wall topWall; topWall.Transform.Position = {0, 6.5f, 0}; topWall.Collider.Size = {32, 1, 0};
		MiniGame::Wall bottomWall; bottomWall.Transform.Position = {0, -6.5f, 0}; bottomWall.Collider.Size = {32, 1, 0};

		m_Walls.emplace_back(leftLowerWall);
		m_Walls.emplace_back(rightLowerWall);
		m_Walls.emplace_back(leftUpperWall);
		m_Walls.emplace_back(rightUpperWall);
		m_Walls.emplace_back(topWall);
		m_Walls.emplace_back(bottomWall);

		for (auto& wall : m_Walls)
		{
			registerBoxCollider(wall.Transform, wall.Collider);
		}

		// Create background object.
		m_BackgroundSprite.Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundSprite.Texture->PixelsPerUnit = 32;
		m_BackgroundTransform.Scale = {64.0f, 64.0f, 1};
		m_BackgroundTransform.Position = {0, 0, -2};

		// Setup main camera for debugging purpose.
		m_MainWindow = WindowManager::GetMainWindow();
		m_MainWindow->CenterWindow();
		m_MainWindow->SetSize(640, 480);

		m_MainCamera.Transform.Position = glm::vec3 {0, 0, 10};
		m_MainCamera.Properties.IsOrthographic = true;
		m_MainCamera.Properties.OrthographicSize = 20;
		m_MainCamera.Properties.TargetType = RenderTargetType::Window;
		m_MainCamera.Properties.TargetWindowID = m_MainWindow->GetWindowID();
		m_MainCamera.Properties.UseManualAspectRatio = false;
		m_MainCamera.Properties.ManualAspectRatio = (float) 1600 / 900;
		m_MainCamera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		m_MainCamera.Properties.Viewport = { 0, 0, 1, 1 };

		// Create window camera.
		m_Player1WindowCamera.CreateWindow(m_MainWindow->GetContext(), WindowProperty("Player 1", 800, 900));
		m_Player1WindowCamera.pWindow->CenterWindow();
		auto position = m_Player1WindowCamera.pWindow->GetPosition();
		position.x -= 410;
		m_Player1WindowCamera.pWindow->SetPosition(position.x, position.y);
		m_Player1WindowCamera.pWindow->SetBorderedIfWindowed(false);

		m_Player2WindowCamera.CreateWindow(m_MainWindow->GetContext(), WindowProperty("Player 2", 800, 900));
		m_Player2WindowCamera.pWindow->CenterWindow();
		position = m_Player2WindowCamera.pWindow->GetPosition();
		position.x += 390;
		m_Player2WindowCamera.pWindow->SetPosition(position.x, position.y);
		m_Player2WindowCamera.pWindow->SetBorderedIfWindowed(false);

		// Set the current context back to the main window.
		m_MainWindow->MakeCurrent();
		ContextBase::SetVSyncCountForCurrentContext(0);

		// Hide the main window by default (as debug window, press F9/F10 to toggle it).
		SDL_MinimizeWindow(m_MainWindow->GetTypedNativeWindowPtr<SDL_Window>());
	}

	void PongLayer::registerBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
	{
		if (collider.ID.has_value() && m_ColliderManager.IsColliderRegistered(collider.ID.value()))
		{
			// The collider has already been registered to the manager.
			return;
		}

		collider.ID = m_ColliderManager.RegisterAABB(Math::AABB::CreateFromCenter(transform.Position, collider.Size));
	}

	void PongLayer::unregisterBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
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

	void PongLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(m_MainCamera.GetTransformedProperties());
		RenderPipelineManager::RegisterCameraForNextRender(m_Player1WindowCamera.Camera.GetTransformedProperties());
		RenderPipelineManager::RegisterCameraForNextRender(m_Player2WindowCamera.Camera.GetTransformedProperties());

		renderSprite(m_Ball.Transform, m_Ball.Sprite);

		// Scroll tiled offset
		float const offsetChange = TIME.DeltaTime() * 0.5f * m_BackgroundScrollingSpeed;
		m_BackgroundSprite.TilingOffset += glm::vec2 {offsetChange, offsetChange};
		if (m_BackgroundSprite.TilingOffset.x > 1.0f)
		{
			m_BackgroundSprite.TilingOffset -= glm::vec2 {1.0f, 1.0f};
		}

		renderSprite(m_BackgroundTransform, m_BackgroundSprite);
	}

	void PongLayer::renderSprite(MiniGame::Transform &transform, MiniGame::Sprite &sprite)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, transform.Position);
		modelMatrix = modelMatrix * glm::toMat4(transform.Rotation);
		modelMatrix = glm::scale(modelMatrix, transform.Scale);

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
			->SubmitTiledSprite(sprite.Texture, {transform.Scale.x * sprite.TilingScale.x, transform.Scale.y * sprite.TilingScale.y, sprite.TilingOffset}, sprite.Color, modelMatrix);
	}

	void PongLayer::OnEvent(Event& event)
	{

	}

	void PongLayer::OnUpdate()
	{
		// Debug updates.
		debugInput();
		m_FPSCounter.NewFrame(TIME.DeltaTime());
		if (m_DrawColliderGizmos)
		{
			m_ColliderManager.DrawGizmos();
			for (auto const& area : m_Homebases)
			{
				DebugDraw::Cube(area.Transform.Position, area.Collider.Size, Color::Yellow);
			}
		}

		// Gameplay updates
		readPaddleInput();

		m_Player1WindowCamera.UpdateWindowResizeAnimation(TIME.DeltaTime());
		m_Player1WindowCamera.UpdateWindowMoveAnimation(TIME.DeltaTime());
		m_Player1WindowCamera.UpdateCameraProperties();

		m_Player2WindowCamera.UpdateWindowResizeAnimation(TIME.DeltaTime());
		m_Player2WindowCamera.UpdateWindowMoveAnimation(TIME.DeltaTime());
		m_Player2WindowCamera.UpdateCameraProperties();
	}

	void PongLayer::debugInput()
	{
		if (INPUT.GetKeyDown(KeyCode::F9))
		{
			SDL_MinimizeWindow(m_MainWindow->GetTypedNativeWindowPtr<SDL_Window>());
		}

		if (INPUT.GetKeyDown(KeyCode::F10))
		{
			SDL_RestoreWindow(m_MainWindow->GetTypedNativeWindowPtr<SDL_Window>());
		}

		if (INPUT.GetKeyDown(KeyCode::Space))
		{
			m_Ball.Velocity.Value += glm::vec2 {5, 0};
		}

		if (INPUT.GetKeyDown(KeyCode::Escape))
		{
			Application::GetRegisteredApplications()[0]->Shutdown();
			return;
		}
		auto mainWindowPtr = WindowManager::GetMainWindow();
		if (INPUT.GetKeyDown(KeyCode::F1))
		{
			mainWindowPtr->CenterWindow();
		}

		if (INPUT.GetKeyDown(KeyCode::F2))
		{
			m_Player2WindowCamera.SmoothResize(1920, 1080);
		}

		if (INPUT.GetKeyDown(KeyCode::F3))
		{
			m_Player2WindowCamera.SmoothResize(1600, 900);
		}

		if (INPUT.GetKeyDown(KeyCode::F4))
		{
			m_Player2WindowCamera.SmoothResize(800, 640);
		}

		if (INPUT.GetKeyDown(KeyCode::F5))
		{
			m_Player2WindowCamera.SmoothResize(640, 640);
		}

		if (INPUT.GetKeyDown(KeyCode::F6))
		{
			m_Player2WindowCamera.SmoothResize(320, 320);
		}

		if (INPUT.GetKeyDown(KeyCode::F7))
		{
			m_Player2WindowCamera.SmoothResize(240, 240);
		}
	}

	void PongLayer::readPaddleInput()
	{
		if (INPUT.IsGamepadConnected(0))
		{
			// Paddle 1
			auto &paddle = m_PlayerPaddles[0];
			float const rightVertical = INPUT.GetGamepadAxis(0, GamepadAxis::RightStickVertical);
			float const leftVertical = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickVertical);

			float const combinedVertical = glm::clamp(rightVertical + leftVertical, -1.0f, 1.0f);
			glm::vec3 const axis = {0, -combinedVertical, 0};

			if (glm::length2(axis) > 0.01f)
			{
				paddle.MovementInputBuffer = axis;
			}

			if (INPUT.GetGamepadButtonDown(0, GamepadButton::LeftStick))
			{
				if (m_Ball.Attachable.IsAttached && m_Ball.Attachable.AttachedPaddle == &paddle)
				{
					m_Ball.LaunchFromAttachedPaddle();
				}
			}
		}
		else
		{
			auto &paddle = m_PlayerPaddles[0];
			if (INPUT.GetKey(KeyCode::W))
			{
				paddle.MovementInputBuffer = glm::vec2 {0, 1};
			}
			else if (INPUT.GetKey(KeyCode::S))
			{
				paddle.MovementInputBuffer = glm::vec2 {0, -1};
			}

			if (INPUT.GetKeyDown(KeyCode::Space))
			{
				if (m_Ball.Attachable.IsAttached && m_Ball.Attachable.AttachedPaddle == &paddle)
				{
					m_Ball.LaunchFromAttachedPaddle();
				}
			}
		}

		if (INPUT.IsGamepadConnected(1))
		{
			// Paddle 2
			auto& paddle = m_PlayerPaddles[1];
			float const rightVertical = INPUT.GetGamepadAxis(1, GamepadAxis::RightStickVertical);
			float const leftVertical = INPUT.GetGamepadAxis(1, GamepadAxis::LeftStickVertical);

			float const combinedVertical = glm::clamp(rightVertical + leftVertical, -1.0f, 1.0f);
			glm::vec3 const axis = {0, -combinedVertical, 0};

			if (glm::length2(axis) > 0.01f)
			{
				paddle.MovementInputBuffer = axis;
			}

			if (INPUT.GetGamepadButtonDown(1, GamepadButton::LeftStick))
			{
				if (m_Ball.Attachable.IsAttached && m_Ball.Attachable.AttachedPaddle == &paddle)
				{
					m_Ball.LaunchFromAttachedPaddle();
				}
			}
		}
		else
		{
			auto &paddle = m_PlayerPaddles[1];
			if (INPUT.GetKey(KeyCode::Up))
			{
				paddle.MovementInputBuffer = glm::vec2 {0, 1};
			}
			else if (INPUT.GetKey(KeyCode::Down))
			{
				paddle.MovementInputBuffer = glm::vec2 {0, -1};
			}

			if (INPUT.GetKeyDown(KeyCode::Return))
			{
				if (m_Ball.Attachable.IsAttached && m_Ball.Attachable.AttachedPaddle == &paddle)
				{
					m_Ball.LaunchFromAttachedPaddle();
				}
			}
		}
	}

	void PongLayer::updateBoxCollider(MiniGame::Transform &transform, MiniGame::BoxCollider &collider)
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

	void PongLayer::OnFixedUpdate()
	{
		auto const timeStep = (float) TIME.FixedDeltaTime();
		for (auto& paddle : m_PlayerPaddles)
		{
			updatePaddle(paddle, timeStep);
		}

		updateBall(timeStep);
	}

	void PongLayer::updatePaddle(MiniGame::PlayerPaddle& paddle, float timeStep)
	{
		float const inputSqr = glm::length2(paddle.MovementInputBuffer);
		if (inputSqr <= glm::epsilon<float>())
		{
			paddle.VelocityBuffer = {0, 0};
			return;
		}

		// Calculate the paddleVelocityForFrame
		float const magnitude = glm::length(paddle.MovementInputBuffer);
		glm::vec3 const direction = glm::normalize(glm::vec3 {paddle.MovementInputBuffer, 0});
		glm::vec3 const paddleVelocity = paddle.Speed * magnitude * direction;
		glm::vec3 const paddleVelocityForFrame = paddleVelocity * timeStep;

		// Calculate the new position for the paddle.
		// Clamp the position & velocity if it's over the max/min constraints.
		auto newPaddlePosition = paddle.Transform.Position + paddleVelocityForFrame;
		bool isPaddleVelocityClamped = false;
		if (newPaddlePosition.y < paddle.MinPositionY)
		{
			isPaddleVelocityClamped = true;
			newPaddlePosition.y = paddle.MinPositionY;
		}
		if (newPaddlePosition.y > paddle.MaxPositionY)
		{
			isPaddleVelocityClamped = true;
			newPaddlePosition.y = paddle.MaxPositionY;
		}

		auto actualPositionOffset = newPaddlePosition - paddle.Transform.Position;
		auto actualPositionOffsetInSecond = actualPositionOffset / timeStep;

		// In case when the velocity is clamped, we will buffer the actual position offset instead.
		paddle.VelocityBuffer = isPaddleVelocityClamped? actualPositionOffsetInSecond : paddleVelocity;

		// Push the ball if there is an overlap.
		Math::DynamicTestResult2D result2D;
		bool const intersectBall = Math::MovingCircleAABBIntersect(m_Ball.Transform.Position, m_Ball.Collider.Radius,
																   -actualPositionOffset, paddle.GetAABB(), result2D);
		if (intersectBall)
		{
			glm::vec2 const normal = glm::normalize(result2D.HitNormal);
			auto ballPaddleDot = glm::dot(actualPositionOffset, glm::vec3 {m_Ball.Velocity.Value, 0});

			if (ballPaddleDot < 0.0f)
			{
				// If the ball is moving in the opposite direction of the paddle,
				// bounce (deflect) the ball into reflected direction.
				m_Ball.Velocity.Value = m_Ball.Velocity.Value - 2 * glm::dot(normal, m_Ball.Velocity.Value) * normal;
				m_Ball.Velocity.Value += glm::vec2 {paddle.VelocityBuffer.x, paddle.VelocityBuffer.y};
			}

			// Move the ball away from the paddle to avoid tunneling
			m_Ball.Transform.Position += actualPositionOffset;
			m_Ball.Hittable.LastHitByPlayerID = paddle.PlayerID;
		}

		// Actually update the paddle and its collider.
		paddle.Transform.Position = newPaddlePosition;
		paddle.MovementInputBuffer = {0, 0};
		updateBoxCollider(paddle.Transform, paddle.Collider);
	}

	void PongLayer::updateBall(float timeStep)
	{
		if (m_Ball.Attachable.AttachedPaddle != nullptr)
		{
			// The ball is attached to a paddle, move the ball with the paddle
			m_Ball.Transform.Position = m_Ball.Attachable.AttachedPaddle->Transform.Position + glm::vec3 {m_Ball.Attachable.AttachOffset, 0};
			return;
		}

		// Ball collision detection.
		glm::vec2 const positionChange = timeStep * m_Ball.Velocity.Value;
		auto const& hits = m_ColliderManager.CircleCastAll(m_Ball.Transform.Position, 0.25f, positionChange);
		if (!hits.empty())
		{
			auto const& hit = hits[0];
			glm::vec2 const normal = glm::normalize(hit.Normal);

			float const minimumTravelTimeAfterReflected = 0.001f;
			float const reflectedTravelTime = glm::max(minimumTravelTimeAfterReflected, timeStep - hit.Time);
			m_Ball.Velocity.Value = m_Ball.Velocity.Value - 2 * glm::dot(normal, m_Ball.Velocity.Value) * normal;
			m_Ball.Transform.Position = glm::vec3(hit.Centroid + reflectedTravelTime * m_Ball.Velocity.Value, 0);

			for (auto const& paddle : m_PlayerPaddles)
			{
				// If the box is a paddle, update velocity based on the paddle's state.
				if (hit.ColliderID != paddle.Collider.ID)
				{
					continue;
				}

				auto paddleVelocity = paddle.VelocityBuffer;

				// Increase the horizontal speed if it's a paddle.
				m_Ball.Velocity.Value.x += glm::sign(m_Ball.Velocity.Value.x) * paddle.HorizontalBallSpeedIncreasePerHit;

				// Add paddle velocity to the ball.
				m_Ball.Velocity.Value += glm::vec2 {paddleVelocity.x, paddleVelocity.y};

				m_Ball.Hittable.LastHitByPlayerID = paddle.PlayerID;

				break;
			}
		}
		else
		{
			m_Ball.Transform.Position += glm::vec3(positionChange, 0);
		}

		// Check if it's a goal.
		for (auto const& homebase : m_Homebases)
		{
			if (!Math::AABBCircleIntersect(homebase.GetAABB(), m_Ball.Transform.Position, m_Ball.Collider.Radius))
			{
				continue;
			}

			// The ball hits a homebase! Reset the state.
			m_Ball.Hittable.LastHitByPlayerID = homebase.PlayerID;

			// Deal 1 damage to the homebase.
			int const playerID = homebase.PlayerID;
			auto playerItr = std::find_if(
								m_Players.begin(),
								m_Players.end(),
								[playerID](MiniGame::PongPlayer const& player)
								{
									return playerID == player.Settings.ID;
								});

			if (playerItr != m_Players.end())
			{
				// Reduce health (earn score).
				playerItr->State.Health--;
			}

			auto paddleItr = std::find_if(
				m_PlayerPaddles.begin(),
				m_PlayerPaddles.end(),
				[playerID](MiniGame::PlayerPaddle const& paddle)
				{
					return playerID == paddle.PlayerID;
				});

			if (paddleItr != m_PlayerPaddles.end())
			{
				// Attach the ball to the paddle!
				m_Ball.EquipToPaddle(*paddleItr, playerItr->Settings.MainPaddleAttachOffset);
			}
			else
			{
				// Reset the ball to the center
				m_Ball.Transform.Position = {0, 0, 0};
				m_Ball.Velocity.Value = {5, 1}; // TODO: randomized the velocity.
			}
			break;
		}
	}

	void PongLayer::OnImGui()
	{
		// get the window size as a base for calculating widgets geometry
		auto mainWindowPtr = WindowManager::GetMainWindow();

		if (ImGui::Begin("General"))
		{
			if (ImGui::CollapsingHeader("System"))
			{
				ImGui::Dummy(ImVec2(0.0f, 1.0f));
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
				ImGui::Text("%s", SDL_GetPlatform());
				ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
				ImGui::Text("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);
				ImGui::Text("DeltaTime: [%f]", TIME.DeltaTime());
				ImGui::Text("FPS: [%f]", m_FPSCounter.GetLastCalculatedFPS());
			}

			if (ImGui::CollapsingHeader("Game State"))
			{
				for (int i = 0; i < m_Players.size(); i++)
				{
					auto& player = m_Players[i];
					ImGuiUtil::DrawUnsignedIntControl("Player " + std::to_string(i) + " Health", player.State.Health, 0);
				}

				if (m_Ball.Attachable.IsAttached)
				{
					ImGuiUtil::DrawReadOnlyTextWithLabel("Ball Attach To", std::to_string(m_Ball.Attachable.AttachedPaddle->PlayerID));
				}
				else
				{
					ImGuiUtil::DrawVec2Control("Ball Velocity", m_Ball.Velocity.Value, 0.0f);
				}
			}

			if (ImGui::CollapsingHeader("Window & Mouse"))
			{
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
				}
			}

			if (ImGui::CollapsingHeader("Rendering"))
			{
				if (ImGui::Button("Line Mode"))
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				ImGui::SameLine();
				if (ImGui::Button("Fill Mode"))
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}

				if (ImGui::Button("Toggle Debug Gizmos"))
				{
					m_DrawColliderGizmos = !m_DrawColliderGizmos;
				}
				ImGuiUtil::DrawFloatControl("BG Scrolling Speed", m_BackgroundScrollingSpeed, 1.0f);
			}

			if (ImGui::CollapsingHeader("World"))
			{
				ImGuiUtil::DrawCameraPropertiesControl("Main Camera Properties", m_MainCamera.Properties);

				ImGui::Separator();
				ImGuiUtil::DrawCameraPropertiesControl("Debug Camera Properties", m_DebugCamera.Properties);

				ImGui::Separator();
				imguiSprite("PongBall", m_Ball.Transform, m_Ball.Sprite);
				ImGui::Separator();
				imguiSprite("Background", m_BackgroundTransform, m_BackgroundSprite);
			}
		}
		ImGui::End();

		m_ColliderManager.DrawImGui();
		INPUT.DrawAllRegisteredDeviceDescriptorsImGui();
		INPUT.DrawAllConnectedDeviceDescriptorsImGui();
	}

	void PongLayer::imguiSprite(const std::string& name, MiniGame::Transform& transform, MiniGame::Sprite& sprite)
	{
		ImGui::PushID(name.c_str());

		ImGuiUtil::DrawVec3Control("Position", transform.Position);
		ImGuiUtil::DrawVec3Control("Scale", transform.Scale);

		glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(transform.Rotation);
		rotationInEulerAnglesDegree += glm::vec3(0.f);
		rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
		if (ImGuiUtil::DrawVec3Control("Rotation", rotationInEulerAnglesDegree))
		{
			rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
			transform.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};
		}

		ImGuiUtil::DrawVec2Control("_TilingScale", sprite.TilingScale, 1.0f);
		ImGuiUtil::DrawVec2Control("_TilingOffset", sprite.TilingOffset, 0.0f);
		ImGuiUtil::DrawColor4Control("_Color", sprite.Color);

		ImGui::PopID();
	}
}