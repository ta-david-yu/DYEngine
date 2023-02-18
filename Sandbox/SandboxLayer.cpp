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

namespace DYE
{
    SandboxLayer::SandboxLayer()
    {
		auto mainWindowPtr = WindowManager::GetMainWindow();
		m_CameraProperties.ClearColor = Color::Black;
		m_CameraProperties.TargetWindowID = mainWindowPtr->GetWindowID();
		m_CameraProperties.ManualAspectRatio = (float) mainWindowPtr->GetWidth() / (float) mainWindowPtr->GetHeight();
		m_CameraProperties.Position = glm::vec3 {0, 0, 10};
    }

    void SandboxLayer::OnUpdate()
    {
		// Movement input system
		if (INPUT.GetKey(KeyCode::D) || INPUT.GetKey(KeyCode::Right))
		{
			m_InputBuffer.x = 1.0f;
		}

		if (INPUT.GetKey(KeyCode::A) || INPUT.GetKey(KeyCode::Left))
		{
			m_InputBuffer.x = -1.0f;
		}

		// Jump input system
		if (INPUT.GetKeyDown(KeyCode::Space))
		{
			// TODO: Magic number :P gonna change it later with a speed value based on jump height & apex time
			m_BallVelocity.y += 10.0f;
		}
    }

    void SandboxLayer::OnFixedUpdate()
    {
		float timeStep = static_cast<float>(TIME.FixedDeltaTime());

		// Movement input action system
		glm::vec2 input = {0, 0};
		if (glm::length2(m_InputBuffer) > 0.01f)
		{
			input = m_InputBuffer;
		}
		m_InputBuffer = {0, 0};
		m_BallVelocity.x = m_HorizontalMoveUnitsPerSecond * input.x;

		// Gravity system
		m_BallVelocity += glm::vec3 {0, m_BallGravity, 0} * timeStep;

		// Movement system
		if (glm::length2(m_BallVelocity) <= 0.01f)
		{
			return;
		}

		Math::AABB const groundAABB = getGroundAABB();
		auto moveOffset = m_BallVelocity * timeStep;

		glm::vec3 horizontalMoveOffset = {moveOffset.x, 0, 0};
		glm::vec3 verticalMoveOffset = {0, moveOffset.y, 0};

		Math::DynamicTestResult2D result2D;

		if (glm::abs(moveOffset.x) > 0)
		{
			// Horizontal collision test
			bool const hitHorizontally = Math::MovingCircleAABBIntersect(m_BallPosition, m_BallRadius,
																		 horizontalMoveOffset, groundAABB, result2D);
			if (hitHorizontally)
			{
				horizontalMoveOffset *= result2D.HitTime;
				//if (horizontalMoveOffset.x >= m_BallRadiusSkin)
				{
					horizontalMoveOffset -= glm::vec3 {glm::sign(horizontalMoveOffset.x), 0, 0} * m_BallRadiusSkin;
				}
			}
		}

		if (glm::abs(moveOffset.y) > 0)
		{
			// Vertical collision test
			bool const hitVertically = Math::MovingCircleAABBIntersect(m_BallPosition, m_BallRadius, verticalMoveOffset,
																	   groundAABB, result2D);
			if (hitVertically)
			{
				verticalMoveOffset *= result2D.HitTime;
				//if (verticalMoveOffset.y >= m_BallRadiusSkin)
				{
					verticalMoveOffset -= glm::vec3 {0, glm::sign(verticalMoveOffset.y), 0} * m_BallRadiusSkin;
				}

				// Hit the ground, set vertical velocity to zero.
				m_BallVelocity.y = 0;
			}
		}

		moveOffset = horizontalMoveOffset + verticalMoveOffset;
		m_BallPosition += moveOffset;

		//m_BallPosition.x = glm::clamp(m_BallPosition.x, -m_GroundWidth * 0.5f, m_GroundWidth * 0.5f);
    }

	void SandboxLayer::OnRender()
	{
		// TODO: Render system instead of im debug draw
		Math::AABB const groundAABB = getGroundAABB();
		DebugDraw::AABB(groundAABB.Min, groundAABB.Max, Color::Yellow);
		DebugDraw::Sphere(m_BallPosition, m_BallRadius, Color::White);

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
				ImGuiUtil::DrawFloatControl("Gravity", m_BallGravity, -9.8f);

				ImGui::BeginDisabled(true);
				ImGuiUtil::DrawVec3Control("Position", m_BallPosition);
				ImGuiUtil::DrawVec3Control("Velocity", m_BallVelocity);
				ImGuiUtil::DrawFloatControl("Radius", m_BallRadius, 0.5f);
				ImGui::EndDisabled();
			}

		}
		ImGui::End();

        ImGui::ShowDemoWindow();
    }
}