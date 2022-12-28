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
	class Material;
	class Texture2D;
	class CameraProperties;

	struct MaterialObject
	{
		std::string Name;

		glm::vec3 Position{0, 0, 0};
		glm::vec3 Scale{1, 1, 1};
		glm::quat Rotation{glm::vec3 {0, 0, 0}};

		std::shared_ptr<Material> Material;
	};

    class PongLayer : public LayerBase
    {
    public:
        explicit PongLayer(WindowBase* pWindow);
		PongLayer() = delete;

		void OnInit() override;
        void OnEvent(Event& event) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
		void OnRender() override;
        void OnImGui() override;

	private:
		void renderMaterialObject(MaterialObject& object);
		void imguiMaterialObject(MaterialObject& object);

    private:
        WindowBase* m_pWindow;
		bool m_IsControlWindowOpen = false;
        double m_FpsAccumulator = 0;
        int m_FramesCounter = 0;
        int m_FixedUpdateCounter = 0;

		int m_KeyDownCounter = 0;
		int m_KeyUpCounter = 0;
		int m_KeyCounter = 0;

		std::shared_ptr<VertexArray> m_VertexArrayObject; // Mesh
		std::shared_ptr<ShaderProgram> m_ShaderProgram; // Material

		std::shared_ptr<MaterialObject> m_ProfileObject;
		std::shared_ptr<MaterialObject> m_WhiteObject;

		std::shared_ptr<CameraProperties> m_CameraProperties;
	};
}