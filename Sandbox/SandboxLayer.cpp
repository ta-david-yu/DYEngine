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
#include "Graphics/RenderPipeline2D.h"
#include "Graphics/Texture.h"

#include <imgui.h>
#include <glm/gtx/norm.hpp>

namespace DYE::Sandbox
{
    SandboxLayer::SandboxLayer()
    {
		auto mainWindowPtr = WindowManager::GetMainWindow();
		m_CameraProperties.ClearColor = Color::Black;
		m_CameraProperties.TargetWindowID = mainWindowPtr->GetWindowID();
		m_CameraProperties.ManualAspectRatio = (float) mainWindowPtr->GetWidth() / (float) mainWindowPtr->GetHeight();
		m_CameraProperties.Position = glm::vec3 {0, 0, 20};
		m_CameraProperties.OrthographicSize = 20;
		m_CameraProperties.IsOrthographic = true;

		recalculateJumpParameters();

		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({0, 0, 0}, {5, 1, 0}));
		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({-3, -3, 0}, {5, 1, 0}));
		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({-3, 3, 0}, {5, 1, 0}));
		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({-5.5f, 0, 0}, {1, 6, 0}));

		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({5, -2, 0}, {1, 8, 0}));
		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({7, -2.5f, 0}, {1, 9, 0}));

		m_StaticColliderManager.RegisterAABB(Math::AABB::CreateFromCenter({0, -7.5f, 0}, {25, 1, 0}));
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

		// We want to shrink the player's AABB by player skin width on each side,
		// therefore we could still cast AABB without starting inside a collider safely when the player is adjacent to the collider.
		Math::AABB const playerAABB = Math::AABB::CreateFromCenter(m_PlayerPosition, glm::vec3{m_PlayerWidth - m_PlayerSkin * 2, m_PlayerHeight- m_PlayerSkin * 2, 1});

		auto moveOffset = m_PlayerVelocity * timeStep;

		float const horizontalDirectionSign = glm::sign(moveOffset.x);
		float horizontalMoveOffset = glm::abs(moveOffset.x) + m_PlayerSkin;

		float const verticalDirectionSign = glm::sign(moveOffset.y);
		float verticalMoveOffset = glm::abs(moveOffset.y) + m_PlayerSkin;

		if (glm::abs(moveOffset.x) > 0)
		{
			// Horizontal collision test
			auto hits = m_StaticColliderManager.AABBCastAll(playerAABB, glm::vec3 {horizontalDirectionSign * horizontalMoveOffset, 0, 0});

			bool const hitHorizontally = !hits.empty();
			if (hitHorizontally)
			{
				horizontalMoveOffset *= hits[0].Time;
				horizontalMoveOffset -= m_PlayerSkin;
			}
		}

		if (glm::abs(moveOffset.y) > 0)
		{
			// Vertical collision test
			auto hits = m_StaticColliderManager.AABBCastAll(playerAABB, glm::vec3 {0, verticalDirectionSign * verticalMoveOffset, 0});

			bool const hitVertically = !hits.empty();
			if (hitVertically)
			{
				verticalMoveOffset *= hits[0].Time;;
				verticalMoveOffset -= m_PlayerSkin;

				// Hit the ground, set vertical velocity to zero.
				m_PlayerVelocity.y = 0;
			}
		}

		moveOffset = {horizontalDirectionSign * horizontalMoveOffset, verticalDirectionSign * verticalMoveOffset, 0};
		if (glm::length2(moveOffset) > 0)
		{
			auto hits = m_StaticColliderManager.AABBCastAll(playerAABB, moveOffset);

			if (!hits.empty())
			{
				moveOffset *= hits[0].Time;
				moveOffset -= glm::vec3 {horizontalDirectionSign * m_PlayerSkin, verticalDirectionSign * m_PlayerSkin, 0};
			}
		}

		m_PlayerPosition += moveOffset;
    }

	void SandboxLayer::OnRender()
	{
		m_StaticColliderManager.DrawGizmos();

		// Camera system
		RenderPipelineManager::RegisterCameraForNextRender(m_CameraProperties);

		// Render system
		auto texture = Texture2D::GetWhiteTexture();
		auto pixelsPerUnit = texture->PixelsPerUnit;

		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, m_PlayerPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3{pixelsPerUnit * m_PlayerWidth, pixelsPerUnit * m_PlayerHeight, 1});

		RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
		    ->SubmitSprite(texture, m_PlayerColor, modelMatrix);
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

			if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGuiUtil::DrawFloatControl("Horizontal Speed (unit/sec)", m_HorizontalMoveUnitsPerSecond, 3.0f);
				ImGuiUtil::Parameters::FloatFormat = "%.4f";
				ImGuiUtil::DrawFloatControl("Player Skin Width", m_PlayerSkin, 0.015f);
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

				ImGuiUtil::DrawColor4Control("Player Color", m_PlayerColor);

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
				ImGuiUtil::DrawFloatControl("Width", m_PlayerWidth, 1);
				ImGuiUtil::DrawFloatControl("Height", m_PlayerHeight, 1);
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