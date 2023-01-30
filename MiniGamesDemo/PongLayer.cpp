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

		player1.State.Health = MaxHealth;

		MiniGame::PongPlayer player2 {};
		player2.Settings.ID = 1;
		player2.Settings.MainPaddleLocation = {10, 0, 0};
		player2.Settings.MainPaddleAttachOffset = {-paddleAttachOffsetX, 0};
		player2.Settings.HomebaseCenter = {14, 0, 0};	// TODO:
		player2.Settings.HomebaseSize = {2, 16, 1}; 	// TODO:

		player2.State.Health = MaxHealth;

		m_Players.emplace_back(player1);
		m_Players.emplace_back(player2);

		// Create paddle objects.
		auto paddleTexture = Texture2D::Create("assets\\Sprite_PongPaddle.png");
		for (auto const& player : m_Players)
		{
			MiniGame::PlayerPaddle paddle;
			paddle.PlayerID = player.Settings.ID;
			paddle.Transform.Position = player.Settings.MainPaddleLocation;
			paddle.Sprite.Texture = paddleTexture;
			paddle.Sprite.Texture->PixelsPerUnit = 32;
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

		MiniGame::Wall topWall; topWall.Transform.Position = {0, 6.5f, 0}; topWall.Collider.Size = {36, 1, 0};
		MiniGame::Wall bottomWall; bottomWall.Transform.Position = {0, -6.5f, 0}; bottomWall.Collider.Size = {36, 1, 0};

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

		m_BorderSprite.Texture = Texture2D::Create("assets\\Sprite_PongBorder.png");
		m_BorderSprite.Texture->PixelsPerUnit = 32;
		m_BorderTransform.Position = {0, 0, 0};

		// Create background object.
		m_BackgroundSprite.Texture = Texture2D::Create("assets\\Sprite_Grid.png");
		m_BackgroundSprite.Texture->PixelsPerUnit = 32;
		m_BackgroundSprite.IsTiled = true;
		m_BackgroundTransform.Scale = {64.0f, 64.0f, 1};
		m_BackgroundTransform.Position = {0, 0, -2};

		// Setup main camera for debugging purpose.
		m_MainWindow = WindowManager::GetMainWindow();
		m_MainWindow->SetSize(1600, 900);
		m_MainWindow->CenterWindow();

		m_MainCamera.Transform.Position = glm::vec3 {0, 0, 10};
		m_MainCamera.Properties.IsOrthographic = true;
		m_MainCamera.Properties.OrthographicSize = 14;
		m_MainCamera.Properties.TargetType = RenderTargetType::Window;
		m_MainCamera.Properties.TargetWindowID = m_MainWindow->GetWindowID();
		m_MainCamera.Properties.UseManualAspectRatio = false;
		m_MainCamera.Properties.ManualAspectRatio = (float) 1600 / 900;
		m_MainCamera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		m_MainCamera.Properties.Viewport = { 0, 0, 1, 1 };

		// Create window camera.
		m_Player1WindowCamera.CreateWindow(m_MainWindow->GetContext(), WindowProperty("Player 1 - Use Right Analog Stick To Move Window", 800, 900));
		m_Player1WindowCamera.GetWindowPtr()->CenterWindow();
		auto position = m_Player1WindowCamera.GetWindowPtr()->GetPosition();
		position.x -= 410;
		m_Player1WindowCamera.GetWindowPtr()->SetPosition(position.x, position.y);
		m_Player1WindowCamera.GetWindowPtr()->SetBorderedIfWindowed(false);
		m_Player1WindowCamera.ResetCachedPosition();

		m_Player2WindowCamera.CreateWindow(m_MainWindow->GetContext(), WindowProperty("Player 2 - Use Right Analog Stick To Move Window", 800, 900));
		m_Player2WindowCamera.GetWindowPtr()->CenterWindow();
		position = m_Player2WindowCamera.GetWindowPtr()->GetPosition();
		position.x += 390;
		m_Player2WindowCamera.GetWindowPtr()->SetPosition(position.x, position.y);
		m_Player2WindowCamera.GetWindowPtr()->SetBorderedIfWindowed(false);
		m_Player2WindowCamera.ResetCachedPosition();

		// Create UI objects.
		m_GameOverUISprite.Texture = Texture2D::Create("assets\\Sprite_GameOver.png");
		m_GameOverUISprite.Texture->PixelsPerUnit = 32;
		m_GameOverUITransform.Scale = {1, 1, 1};
		m_GameOverUITransform.Position = {0, -1.5f, -1};

		m_WinnerUITransform.Scale = {1, 1, 1};
		m_WinnerUITransform.Position = {0, 0.5f, -1};
		m_P1WinsTexture = Texture2D::Create("assets\\Sprite_P1Wins.png");
		m_P1WinsTexture->PixelsPerUnit = 32;
		m_P2WinsTexture = Texture2D::Create("assets\\Sprite_P2Wins.png");
		m_P2WinsTexture->PixelsPerUnit = 32;
		m_WinnerUISprite.Texture = m_P1WinsTexture;

		// Set the current context back to the main window.
		m_MainWindow->MakeCurrent();
		ContextBase::SetVSyncCountForCurrentContext(0);

		// Equip the ball to the first paddle.
		m_Ball.EquipToPaddle(m_PlayerPaddles[0], m_Players[0].Settings.MainPaddleAttachOffset);

		// Hide the main window by default (for debugging, press F9/F10 to toggle it).
		m_MainWindow->Minimize();
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
		renderSprite(m_BorderTransform, m_BorderSprite);
		for (auto& paddle : m_PlayerPaddles)
		{
			renderSprite(paddle.Transform, paddle.Sprite);
		}

		// Scroll tiled offset
		float const offsetChange = TIME.DeltaTime() * 0.5f * m_BackgroundScrollingSpeed;
		m_BackgroundSprite.TilingOffset += glm::vec2 {offsetChange, offsetChange};
		if (m_BackgroundSprite.TilingOffset.x > 1.0f)
		{
			m_BackgroundSprite.TilingOffset -= glm::vec2 {1.0f, 1.0f};
		}

		renderSprite(m_BackgroundTransform, m_BackgroundSprite);

		// Render GameOver UI sprites
		if (m_GameState == GameState::GameOver)
		{
			renderSprite(m_GameOverUITransform, m_GameOverUISprite);
			renderSprite(m_WinnerUITransform, m_WinnerUISprite);
		}
	}

	void PongLayer::renderSprite(MiniGame::Transform &transform, MiniGame::Sprite &sprite)
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
		readPlayerInput(TIME.DeltaTime());

		auto player1WindowAnimationResult =  m_Player1WindowCamera.UpdateWindowResizeAnimation(TIME.DeltaTime());
		auto player2WindowAnimationResult = m_Player2WindowCamera.UpdateWindowResizeAnimation(TIME.DeltaTime());

		if (m_GameState == GameState::Intermission)
		{
			bool const window1Complete = player1WindowAnimationResult == MiniGame::WindowCamera::AnimationUpdateResult::Complete ||
											player1WindowAnimationResult == MiniGame::WindowCamera::AnimationUpdateResult::Idle;
			bool const window2Complete = player2WindowAnimationResult == MiniGame::WindowCamera::AnimationUpdateResult::Complete ||
											player2WindowAnimationResult == MiniGame::WindowCamera::AnimationUpdateResult::Idle;

			if (window1Complete && window2Complete)
			{
				// Resume to playing state after the intermission animation is completed.
				m_GameState = GameState::Playing;
				m_Ball.PlayRespawnAnimation();
				if (m_pNextPaddleToSpawnBallAfterIntermission != nullptr && m_pNextPlayerToSpawnBall != nullptr)
				{
					// Attach the ball to the paddle!
					m_Ball.EquipToPaddle(*m_pNextPaddleToSpawnBallAfterIntermission, m_pNextPlayerToSpawnBall->Settings.MainPaddleAttachOffset);
				}
				else
				{
					DYE_LOG_ERROR("Next spawn ball paddle / player is nullptr, something is wrong.");

					// Reset the ball to the center
					m_Ball.Transform.Position = {0, 0, 0};
					m_Ball.Velocity.Value = {5, 1};
				}
			}
		}

		m_Player1WindowCamera.UpdateCameraProperties();
		m_Player2WindowCamera.UpdateCameraProperties();

		// Animation updates
		m_Ball.UpdateAnimation(TIME.DeltaTime());

		if (m_GameState == GameState::GameOver)
		{
			// Slow down the scrolling if the game is over.
			if (m_BackgroundScrollingSpeed <= 0.0f)
			{
				m_BackgroundScrollingSpeed = 0.0f;
			}
			else
			{
				m_BackgroundScrollingSpeed -= TIME.DeltaTime();
			}
		}
	}

	void PongLayer::debugInput()
	{
		if (INPUT.GetKeyDown(KeyCode::F9))
		{
			m_MainWindow->Minimize();
		}

		if (INPUT.GetKeyDown(KeyCode::F10))
		{
			m_MainWindow->Restore();
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

	void PongLayer::readPlayerInput(float timeStep)
	{
		// Player 1
		{
			auto &paddle = m_PlayerPaddles[0];
			auto &player = m_Players[0];
			MiniGame::WindowCamera &playerWindowCamera = m_Player1WindowCamera;
			if (INPUT.IsGamepadConnected(0))
			{
				float const vertical = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickVertical);
				glm::vec3 const axis = {0, -vertical, 0};

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

				if (m_GameState != GameState::Intermission && player.State.CanMoveWindow)
				{
					float const windowVertical = INPUT.GetGamepadAxis(0, GamepadAxis::RightStickVertical);
					float const windowHorizontal = INPUT.GetGamepadAxis(0, GamepadAxis::RightStickHorizontal);
					glm::vec2 windowAxis = {windowHorizontal, windowVertical};
					if (glm::length2(windowAxis) > 0.01f)
					{
						if (glm::length2(windowAxis) > 1.0f)
						{
							windowAxis = glm::normalize(windowAxis);
						}
						glm::vec2 const positionChange = windowAxis * playerWindowCamera.MoveSpeed * timeStep;
						playerWindowCamera.Translate(positionChange);
					}
				}
			}
			else
			{
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

				if (m_GameState != GameState::Intermission && player.State.CanMoveWindow)
				{
					float windowVertical = 0;
					if (INPUT.GetKey(KeyCode::T))
					{
						windowVertical = 1;
					}
					else if (INPUT.GetKey(KeyCode::G))
					{
						windowVertical = -1;
					}

					float windowHorizontal = 0;
					if (INPUT.GetKey(KeyCode::H))
					{
						windowHorizontal = 1;
					}
					else if (INPUT.GetKey(KeyCode::F))
					{
						windowHorizontal = -1;
					}

					glm::vec2 windowAxis = {windowHorizontal, -windowVertical};
					if (glm::length2(windowAxis) > 0.01f)
					{
						if (glm::length2(windowAxis) > 1.0f)
						{
							windowAxis = glm::normalize(windowAxis);
						}
						glm::vec2 const positionChange = windowAxis * playerWindowCamera.MoveSpeed * timeStep;
						playerWindowCamera.Translate(positionChange);
					}
				}
			}
		}

		// Player 2
		{
			auto &paddle = m_PlayerPaddles[1];
			auto &player = m_Players[1];
			auto &playerWindowCamera = m_Player2WindowCamera;
			if (INPUT.IsGamepadConnected(1))
			{
				float const vertical = INPUT.GetGamepadAxis(1, GamepadAxis::LeftStickVertical);
				glm::vec3 const axis = {0, -vertical, 0};

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

				if (m_GameState != GameState::Intermission && player.State.CanMoveWindow)
				{
					float const windowVertical = INPUT.GetGamepadAxis(1, GamepadAxis::RightStickVertical);
					float const windowHorizontal = INPUT.GetGamepadAxis(1, GamepadAxis::RightStickHorizontal);
					glm::vec2 windowAxis = {windowHorizontal, windowVertical};
					if (glm::length2(windowAxis) > 0.01f)
					{
						if (glm::length2(windowAxis) > 1.0f)
						{
							windowAxis = glm::normalize(windowAxis);
						}
						glm::vec2 const positionChange = windowAxis * playerWindowCamera.MoveSpeed * timeStep;
						playerWindowCamera.Translate(positionChange);
					}
				}
			}
			else
			{
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

				if (m_GameState != GameState::Intermission && player.State.CanMoveWindow)
				{
					float windowVertical = 0;
					if (INPUT.GetKey(KeyCode::Numpad5))
					{
						windowVertical = 1;
					}
					else if (INPUT.GetKey(KeyCode::Numpad2))
					{
						windowVertical = -1;
					}

					float windowHorizontal = 0;
					if (INPUT.GetKey(KeyCode::Numpad3))
					{
						windowHorizontal = 1;
					}
					else if (INPUT.GetKey(KeyCode::Numpad1))
					{
						windowHorizontal = -1;
					}

					glm::vec2 windowAxis = {windowHorizontal, -windowVertical};
					if (glm::length2(windowAxis) > 0.01f)
					{
						if (glm::length2(windowAxis) > 1.0f)
						{
							windowAxis = glm::normalize(windowAxis);
						}
						glm::vec2 const positionChange = windowAxis * playerWindowCamera.MoveSpeed * timeStep;
						playerWindowCamera.Translate(positionChange);
					}
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

		if (m_GameState == GameState::Playing)
		{
			checkIfBallHasReachedGoal(timeStep);
		}
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
			m_Ball.Velocity.Value.x += glm::sign(m_Ball.Velocity.Value.x) * paddle.HorizontalBallSpeedIncreasePerHit;
			m_Ball.Transform.Position += actualPositionOffset;
			m_Ball.Hittable.LastHitByPlayerID = paddle.PlayerID;
			m_Ball.PlayHitAnimation();
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
				m_Ball.PlayHitAnimation();

				break;
			}
		}
		else
		{
			m_Ball.Transform.Position += glm::vec3(positionChange, 0);
		}
	}

	void PongLayer::checkIfBallHasReachedGoal(float timeStep)
	{
		// Check if it's a goal.
		for (auto const& homebase : m_Homebases)
		{
			if (!Math::AABBCircleIntersect(homebase.GetAABB(), m_Ball.Transform.Position, m_Ball.Collider.Radius))
			{
				continue;
			}

			int const hitPlayerID = m_Ball.Hittable.LastHitByPlayerID;
			auto hitPlayerItr = std::find_if(
				m_Players.begin(),
				m_Players.end(),
				[hitPlayerID](MiniGame::PongPlayer const& player)
				{
					return hitPlayerID == player.Settings.ID;
				});

			// Deal 1 damage to the homebase.
			int const damagedPlayerID = homebase.PlayerID;
			auto playerItr = std::find_if(
				m_Players.begin(),
				m_Players.end(),
				[damagedPlayerID](MiniGame::PongPlayer const& player)
				{
					return damagedPlayerID == player.Settings.ID;
				});

			if (playerItr != m_Players.end())
			{
				m_pNextPlayerToSpawnBall = &(*playerItr);

				// Reduce health (earn score).
				playerItr->State.Health--;

				if (playerItr->State.Health == 0)
				{
					m_GameState = GameState::GameOver;
					m_MainWindow->Restore();

					m_WinnerUISprite.Texture = damagedPlayerID == 0? m_P2WinsTexture : m_P1WinsTexture;
				}
				else
				{
					m_GameState = GameState::Intermission;

					// Shrink the winning player's window size.
					MiniGame::WindowCamera& windowCamera = hitPlayerID == 0? m_Player1WindowCamera : m_Player2WindowCamera;
					auto size = m_HealthWindowSizes[(MaxHealth - 1) - playerItr->State.Health];
					windowCamera.SmoothResize(size.x, size.y);

					// Enable window control/show border if the window shrinks.
					if (playerItr->State.Health <= HealthToEnableWindowInput)
					{
						windowCamera.GetWindowPtr()->SetBorderedIfWindowed(true);
						hitPlayerItr->State.CanMoveWindow = true;
					}
				}
			}
			else
			{
				m_pNextPlayerToSpawnBall = nullptr;
			}

			auto paddleItr = std::find_if(
				m_PlayerPaddles.begin(),
				m_PlayerPaddles.end(),
				[damagedPlayerID](MiniGame::PlayerPaddle const& paddle)
				{
					return damagedPlayerID == paddle.PlayerID;
				});

			if (paddleItr != m_PlayerPaddles.end())
			{
				m_pNextPaddleToSpawnBallAfterIntermission = &(*paddleItr);
			}
			else
			{
				m_pNextPaddleToSpawnBallAfterIntermission = nullptr;
			}

			// Play animations.
			m_Ball.PlayGoalAnimation();

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
				std::string state;
				if (m_GameState == GameState::Playing)
				{
					state = "Playing";
				}
				else if (m_GameState == GameState::Intermission)
				{
					state = "Intermission";
				}
				else if (m_GameState == GameState::GameOver)
				{
					state = "GameOver";
				}

				ImGuiUtil::DrawReadOnlyTextWithLabel("Game State", state);

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

				if (ImGui::Button("Play Ball Respawn Animation"))
				{
					m_Ball.PlayRespawnAnimation();
				}

				ImGui::SameLine();
				if (ImGui::Button("Play Ball Hit Animation"))
				{
					m_Ball.PlayHitAnimation();
				}

				ImGui::SameLine();
				if (ImGui::Button("Play Ball Goal Animation"))
				{
					m_Ball.PlayGoalAnimation();
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