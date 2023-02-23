#include "SandboxLayer.h"

#include "Util/Logger.h"
#include "Util/Macro.h"
#include "Util/Time.h"
#include "Math/Color.h"
#include "Math/PrimitiveTest.h"
#include "ImGui/ImGuiUtil.h"

#include "Input/InputManager.h"

#include "Graphics/WindowManager.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/Texture.h"
#include "Graphics/DebugDraw.h"

#include <imgui.h>
#include <glm/gtx/norm.hpp>

#define DYE_COMPONENT(name)
#define DYE_PROPERTY(name)

namespace DYE::Sandbox
{
	DYE_COMPONENT("ComponentA")
	struct ComponentA
	{
		DYE_PROPERTY("Value")
		int Value;
	};


    SandboxLayer::SandboxLayer()
    {
		int integer = 5;
		auto integer32 = static_cast<std::uint32_t>(integer);

		auto mainWindowPtr = WindowManager::GetMainWindow();
		m_CameraProperties.ClearColor = Color::Black;
		m_CameraProperties.TargetWindowID = mainWindowPtr->GetWindowID();
		m_CameraProperties.ManualAspectRatio = (float) mainWindowPtr->GetWidth() / (float) mainWindowPtr->GetHeight();
		m_CameraProperties.Position = glm::vec3 {0, 0, 20};
		m_CameraProperties.OrthographicSize = 20;
		m_CameraProperties.IsOrthographic = true;

		recalculateJumpParameters();

		Math::AABB groundAABB = getGroundAABB();
		m_CollisionManager.RegisterAABB(groundAABB);

		/*
		groundAABB.Min += glm::vec3 {3, 4, 0};
		groundAABB.Max += glm::vec3 {3, 4, 0};
		m_CollisionManager.RegisterAABB(groundAABB);


		groundAABB.Min += glm::vec3 {-4, -4, 0};
		groundAABB.Max += glm::vec3 {-12, 4, 0};
		m_CollisionManager.RegisterAABB(groundAABB);*/
    }

    void SandboxLayer::OnUpdate()
    {
		// Movement input system
		if (INPUT.GetKey(KeyCode::D) || INPUT.GetKey(KeyCode::Right))
		{
			m_MovementInputBuffer.x = 1.0f;
		}

		if (INPUT.GetKey(KeyCode::A) || INPUT.GetKey(KeyCode::Left))
		{
			m_MovementInputBuffer.x = -1.0f;
		}

		// Jump input system
		if (INPUT.GetKeyDown(KeyCode::Space))
		{
			m_IsJumpPressed = true;
		}

		m_IsJumpHeld = INPUT.GetKey(KeyCode::Space);


		if (INPUT.IsGamepadConnected(0))
		{
			m_MovementInputBuffer.x = INPUT.GetGamepadAxis(0, GamepadAxis::LeftStickHorizontal);

			if (INPUT.GetGamepadButtonDown(0, GamepadButton::South))
			{
				m_IsJumpPressed = true;
			}

			m_IsJumpHeld = INPUT.GetGamepadButton(0, GamepadButton::South);
		}
    }

    void SandboxLayer::OnFixedUpdate()
    {
		auto const timeStep = static_cast<float>(TIME.FixedDeltaTime());

		// Movement input action system
		glm::vec2 input = {0, 0};
		if (glm::length2(m_MovementInputBuffer) > 0.01f)
		{
			input = m_MovementInputBuffer;
		}
		m_MovementInputBuffer = {0, 0};
		m_PlayerVelocity.x = m_HorizontalMoveUnitsPerSecond * input.x;

		// Gravity system
		m_PlayerVelocity += glm::vec3 {0, m_PlayerGravity, 0} * timeStep;

		// Jump input action system
		if (!m_IsJumpHeld && m_PlayerVelocity.y > m_PlayerMinJumpSpeed)
		{
			// Variable jump height: set the speed to min jump speed if the player releases jump button earlier.
			m_PlayerVelocity.y = m_PlayerMinJumpSpeed;
		}

		if (m_IsJumpPressed)
		{
			m_IsJumpPressed = false;
			m_PlayerVelocity.y = m_PlayerMaxJumpSpeed;
		}

		// Movement system
		if (glm::length2(m_PlayerVelocity) <= 0.01f)
		{
			return;
		}

		auto moveOffset = m_PlayerVelocity * timeStep;

		glm::vec3 horizontalMoveOffset = {moveOffset.x, 0, 0};
		glm::vec3 verticalMoveOffset = {0, moveOffset.y, 0};

		Math::AABB const playerAABB = Math::AABB::CreateFromCenter(m_PlayerPosition, glm::vec3{m_PlayerWidth, m_PlayerHeight, 1});

		if (glm::abs(moveOffset.x) > 0)
		{
			// Horizontal collision test
			float const directionSign = glm::sign(moveOffset.x);

			Math::AABB horizontalPlayerAABB = playerAABB;
			horizontalPlayerAABB.Min += directionSign * glm::vec3 {m_PlayerSkin, 0, 0};
			horizontalPlayerAABB.Max += directionSign * glm::vec3 {m_PlayerSkin, 0, 0};

			auto hits = m_CollisionManager.AABBCastAll(horizontalPlayerAABB, horizontalMoveOffset);

			bool const hitHorizontally = !hits.empty();
			if (hitHorizontally)
			{
				horizontalMoveOffset *= hits[0].Time;
			}
		}

		if (glm::abs(moveOffset.y) > 0)
		{
			// Vertical collision test
			float const directionSign = glm::sign(moveOffset.y);

			Math::AABB verticalBallAABB = playerAABB;
			verticalBallAABB.Min += directionSign * glm::vec3 {0, m_PlayerSkin, 0};
			verticalBallAABB.Max += directionSign * glm::vec3 {0, m_PlayerSkin, 0};

			auto hits = m_CollisionManager.AABBCastAll(verticalBallAABB, verticalMoveOffset);

			bool const hitVertically = !hits.empty();
			if (hitVertically)
			{
				verticalMoveOffset *= hits[0].Time;

				// Hit the ground, set vertical velocity to zero.
				m_PlayerVelocity.y = 0;
			}
		}

		moveOffset = horizontalMoveOffset + verticalMoveOffset;
		m_PlayerPosition += moveOffset;

		//m_BallPosition.x = glm::clamp(m_BallPosition.x, -m_GroundWidth * 0.5f, m_GroundWidth * 0.5f);
    }

	void SandboxLayer::OnRender()
	{
		// TODO: Render system instead of im debug draw
		m_CollisionManager.DrawGizmos();
		DebugDraw::Sphere(m_PlayerPosition, m_PlayerRadius, Color::White);

		// DEBUG
		Math::AABB const playerAABB = Math::AABB::CreateFromCenter(m_PlayerPosition, glm::vec3{m_PlayerWidth, m_PlayerHeight, 1});
		Math::AABB verticalPlayerAABB = playerAABB;
		verticalPlayerAABB.Min += -1.0f * glm::vec3 {0, m_PlayerSkin, 0};
		verticalPlayerAABB.Max += -1.0f * glm::vec3 {0, m_PlayerSkin, 0};

		auto hits = m_CollisionManager.AABBCastAll(verticalPlayerAABB, glm::vec3{0, -50.0f, 0});
		if (!hits.empty())
		{
			auto hitAABB = Math::AABB::CreateFromCenter({hits[0].Centroid, 0}, glm::vec3{m_PlayerWidth, m_PlayerHeight, 1});
			DebugDraw::AABB(hitAABB.Min, hitAABB.Max, Color::Yellow);

			DebugDraw::Sphere({hits[0].Centroid, 0}, 0.1f, Color::Red);
		}

		// Camera system
		RenderPipelineManager::RegisterCameraForNextRender(m_CameraProperties);
	}

    void SandboxLayer::OnImGui()
    {
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Sandbox Debug Window"))
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGuiUtil::DrawCameraPropertiesControl("Camera", m_CameraProperties);
			}

			if (ImGui::CollapsingHeader("Ball", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGuiUtil::DrawFloatControl("Horizontal Speed (unit/sec)", m_HorizontalMoveUnitsPerSecond, 3.0f);
				ImGuiUtil::Parameters::FloatFormat = "%.4f";
				ImGuiUtil::DrawFloatControl("Radius Skin", m_PlayerSkin, 0.015f);
				ImGuiUtil::Parameters::FloatFormat = ImGuiUtil::Parameters::DefaultFloatFormat;
				ImGui::Spacing();
				bool requireJumpParametersRecalculation = false;
				requireJumpParametersRecalculation |= ImGuiUtil::DrawFloatControl("Time To Reach Apex", m_PlayerTimeToReachApex, 2);
				requireJumpParametersRecalculation |= ImGuiUtil::DrawFloatControl("Max Jump Height", m_PlayerMaxJumpHeight, 5);
				requireJumpParametersRecalculation |= ImGuiUtil::DrawFloatControl("Min Jump Height", m_PlayerMinJumpHeight, 2);

				if (requireJumpParametersRecalculation)
				{
					recalculateJumpParameters();
				}

				ImGui::Separator();
				static bool disableRuntimeVariable = true;
				ImGuiUtil::DrawBooleanControl("Disable Runtime Variable", disableRuntimeVariable);
				ImGui::BeginDisabled(disableRuntimeVariable);
				ImGuiUtil::DrawFloatControl("Gravity", m_PlayerGravity, -9.8f);
				ImGuiUtil::DrawFloatControl("MaxJumpSpeed", m_PlayerMaxJumpHeight, 10);
				ImGuiUtil::DrawFloatControl("MinJumpSpeed", m_PlayerMinJumpHeight, 2);
				ImGui::Spacing();
				ImGuiUtil::DrawVec3Control("Position", m_PlayerPosition);
				ImGuiUtil::DrawVec3Control("Velocity", m_PlayerVelocity);
				ImGuiUtil::DrawFloatControl("Radius", m_PlayerRadius, 0.5f);
				ImGui::EndDisabled();
			}

		}
		ImGui::End();

		INPUT.DrawInputManagerImGui();
    }

	void SandboxLayer::recalculateJumpParameters()
	{
		m_PlayerGravity = -m_PlayerMaxJumpHeight / (m_PlayerTimeToReachApex * m_PlayerTimeToReachApex * 0.5f);
		m_PlayerMaxJumpSpeed = glm::abs(m_PlayerGravity) * m_PlayerTimeToReachApex;
		m_PlayerMinJumpSpeed = glm::sqrt(2 * glm::abs(m_PlayerGravity) * m_PlayerMinJumpHeight);
	}
}