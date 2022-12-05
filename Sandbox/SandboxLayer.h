#pragma once

#include "LayerBase.h"
#include "Event/KeyEvent.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace DYE
{
    class WindowBase;
	class VertexArray;
	class ShaderProgram;
	class Texture2D;

    class SandboxLayer : public LayerBase
    {
    public:
        explicit SandboxLayer(WindowBase* pWindow);
        SandboxLayer() = delete;

        void OnEvent(Event& event) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
		void OnRender() override;
        void OnImGui() override;
    private:
        WindowBase* m_pWindow;
		bool m_IsControlWindowOpen = false;
        double m_FpsAccumulator = 0;
        int m_FramesCounter = 0;
        int m_FixedUpdateCounter = 0;

		std::shared_ptr<VertexArray> m_VertexArrayObject; // Mesh
		std::shared_ptr<ShaderProgram> m_ShaderProgram; // Material
		std::shared_ptr<Texture2D> m_DefaultTexture; // Material
		glm::vec4 m_Color {1, 1, 1, 1}; // Material

		glm::vec3 m_ObjectPosition{0, 0, 0}; // Entity/Object Transform
		glm::vec3 m_ObjectScale{1, 1, 1}; // Entity/Object Transform
		glm::quat m_ObjectRotation{glm::vec3 {0, 0, 0}}; // Entity/Object Transform



		glm::vec3 m_CameraPosition{0, 0, 3}; // Camera Object Transform
		float m_FieldOfView = 45; // Camera
		float m_NearClipDistance = 0.1f; // Camera
		float m_FarClipDistance = 100; // Camera
		float m_OrthographicCameraSize = 1; // Camera
	};
}