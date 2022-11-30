#pragma once

#include "LayerBase.h"
#include "Event/KeyEvent.h"

#include <glm/glm.hpp>

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

		std::shared_ptr<VertexArray> m_VertexArrayObject;
		std::shared_ptr<ShaderProgram> m_ShaderProgram;
		std::shared_ptr<Texture2D> m_DefaultTexture;

		glm::vec3 m_ObjectPosition{0, 0, 0};
		glm::vec3 m_ObjectScale{1, 1, 1};
		float m_ObjectZRotationInDegree = 0;

		glm::vec3 m_CameraPosition{0, 0, 3};
		float m_FieldOfView = 45;
		float m_NearClipDistance = 0.1f;
		float m_FarClipDistance = 100;
	};
}