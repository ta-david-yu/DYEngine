#pragma once

#include "Core/LayerBase.h"

#include "Graphics/CameraProperties.h"
#include "Math/AABB.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
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
		CameraProperties m_CameraProperties;

		glm::vec2 m_InputBuffer {0, 0};
		float m_HorizontalMoveUnitsPerSecond = 3.0f;

		glm::vec3 m_BallPosition {0, 0, 0};
		glm::vec3 m_BallVelocity {0, 0, 0};
		float m_BallGravity = -9.8f;
		float m_BallRadius = 0.5f;

		float m_GroundY = -5.0f;
		float m_GroundWidth = 10.0f;

		inline Math::AABB getGroundAABB() const { return Math::AABB::CreateFromCenter(glm::vec3{0, m_GroundY, 0}, glm::vec3{m_GroundWidth, 1, 1}); }
	};
}