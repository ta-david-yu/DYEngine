#include "SandboxLayer.h"

#include "Util/Logger.h"
#include "Util/Macro.h"
#include "Util/Time.h"
#include "Math/Color.h"
#include "ImGui/ImGuiUtil.h"

#include "Input/InputManager.h"

#include "Graphics/WindowManager.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/Texture.h"
#include "Graphics/DebugDraw.h"

#include <imgui.h>

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
		if (INPUT.GetKey(KeyCode::D) || INPUT.GetKey(KeyCode::Right))
		{
			m_InputBuffer.x = 1.0f;
		}

		if (INPUT.GetKey(KeyCode::A) || INPUT.GetKey(KeyCode::Left))
		{
			m_InputBuffer.x = -1.0f;
		}

		if (INPUT.GetKey(KeyCode::W) || INPUT.GetKey(KeyCode::Up))
		{
			m_InputBuffer.y = 1.0f;
		}

		if (INPUT.GetKey(KeyCode::S) || INPUT.GetKey(KeyCode::Down))
		{
			m_InputBuffer.y = -1.0f;
		}
    }

    void SandboxLayer::OnFixedUpdate()
    {
		m_BallVelocity.x = m_HorizontalMoveUnitsPerSecond * m_InputBuffer.x;
		m_BallVelocity.y = m_HorizontalMoveUnitsPerSecond * m_InputBuffer.y;

		m_BallPosition += m_BallVelocity * static_cast<float>(TIME.FixedDeltaTime());

		m_InputBuffer = {0, 0};
    }

	void SandboxLayer::OnRender()
	{
		DebugDraw::Sphere(m_BallPosition, m_BallRadius, Color::White);
		DebugDraw::AABB(
			m_BallPosition + glm::vec3 {m_BallRadius, m_BallRadius, m_BallRadius},
			m_BallPosition - glm::vec3 {m_BallRadius, m_BallRadius, m_BallRadius},
			Color::Yellow);
		RenderPipelineManager::RegisterCameraForNextRender(m_CameraProperties);
	}

    void SandboxLayer::OnImGui()
    {
		if (ImGui::Begin("Sandbox Debug Window"))
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGuiUtil::DrawCameraPropertiesControl("Camera", m_CameraProperties);
			}

		}
		ImGui::End();

        ImGui::ShowDemoWindow();
    }
}