#pragma once

#include "Core/LayerBase.h"

#include "Graphics/Camera.h"
#include "Math/AABB.h"

#include "StaticAABBColliderManager.h"

#include "glm/glm.hpp"

namespace DYE::Sandbox
{
    class SandboxLayer : public LayerBase
    {
    public:
        SandboxLayer();
        SandboxLayer(SandboxLayer const& other) = delete;

		void OnAttach() override;

        void OnUpdate() override;
        void OnFixedUpdate() override;
		void OnRender() override;
        void OnImGui() override;

    private:
		StaticAABBColliderManager m_StaticColliderManager;

		Camera m_Camera;
		CameraProperties m_CameraProperties;

		glm::vec2 m_MovementInputBuffer {0, 0};
		bool m_IsJumpPressed = false;
		bool m_IsJumpHeld = false;

		float m_HorizontalMoveUnitsPerSecond = 6.0f;

		glm::vec3 m_PlayerPosition {0, 5, 0};
		glm::vec3 m_PlayerVelocity {0, 0, 0};
		float m_PlayerTimeToReachApex = 0.5f;
		float m_PlayerMaxJumpHeight = 5;
		float m_PlayerMinJumpHeight = 2;

		float m_PlayerMaxJumpSpeed = 10;
		float m_PlayerMinJumpSpeed = 2;
		float m_PlayerGravity = -9.8f;
		float m_PlayerSkin = 0.015f;

		float m_PlayerWidth = 1;
		float m_PlayerHeight = 1;
		glm::vec4 m_PlayerColor {1, 1, 1, 1};

		float m_GroundY = -0.5f;
		float m_GroundWidth = 10.0f;

		void recalculateJumpParameters();
		inline Math::AABB getGroundAABB() const { return Math::AABB::CreateFromCenter(glm::vec3{0, m_GroundY, 0}, glm::vec3{m_GroundWidth, 1, 1}); }
	};
}