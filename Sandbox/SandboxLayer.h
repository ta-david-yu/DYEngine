#pragma once

#include "Core/LayerBase.h"

#include "Graphics/CameraProperties.h"
#include "Math/AABB.h"

#include "CollisionManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE::Sandbox
{
    class SandboxLayer : public LayerBase
    {
    public:
        SandboxLayer();
        SandboxLayer(SandboxLayer const& other) = delete;

        void OnUpdate() override;
        void OnFixedUpdate() override;
		void OnRender() override;
        void OnImGui() override;

    private:
		ColliderManager m_CollisionManager;

		CameraProperties m_CameraProperties;

		glm::vec2 m_MovementInputBuffer {0, 0};
		bool m_IsJumpPressed = false;
		bool m_IsJumpHeld = false;

		float m_HorizontalMoveUnitsPerSecond = 6.0f;

		glm::vec3 m_BallPosition {0, 5, 0};
		glm::vec3 m_BallVelocity {0, 0, 0};
		float m_BallTimeToReachApex = 0.5f;
		float m_BallMaxJumpHeight = 5;
		float m_BallMinJumpHeight = 2;

		float m_BallMaxJumpSpeed = 10;
		float m_BallMinJumpSpeed = 2;
		float m_BallGravity = -9.8f;
		float m_BallRadius = 0.5f;
		float m_BallRadiusSkin = 0.015f;

		float m_GroundY = -2.0f;
		float m_GroundWidth = 10.0f;

		void recalculateJumpParameters();
		inline Math::AABB getGroundAABB() const { return Math::AABB::CreateFromCenter(glm::vec3{0, m_GroundY, 0}, glm::vec3{m_GroundWidth, 1, 1}); }
	};
}