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

	struct SpriteObject
	{
		std::string Name;

		glm::vec3 Position{0, 0, 0};
		glm::vec3 Scale{1, 1, 1};
		glm::quat Rotation{glm::vec3 {0, 0, 0}};

		std::shared_ptr<Texture2D> Texture;
		glm::vec4 Color {1, 1, 1, 1};
	};

    class PongLayer : public LayerBase
    {
    public:
        PongLayer();
		PongLayer(PongLayer const& other) = delete;

		void OnInit() override;
        void OnEvent(Event& event) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
		void OnRender() override;
        void OnImGui() override;

	private:
		static void renderMaterialObject(SpriteObject& object);
		void imguiMaterialObject(SpriteObject& object);

    private:
		bool m_IsControlWindowOpen = false;
        double m_FpsAccumulator = 0;
        int m_FramesCounter = 0;
        int m_FixedUpdateCounter = 0;

		WindowBase* m_SecondWindow = nullptr;

		glm::vec2 m_WindowPosition;
		float m_TargetWindowWidth = 1600;
		float m_TargetWindowHeight = 900;

		float m_WindowPixelChangePerSecond = 300.0f;

		float m_BallSpeed = 5.0f;

		std::shared_ptr<SpriteObject> m_OriginObject;
		std::shared_ptr<SpriteObject> m_AverageObject;
		std::shared_ptr<SpriteObject> m_MovingObject;

		std::vector<std::shared_ptr<SpriteObject>> m_CoordinateObjects;

		std::shared_ptr<CameraProperties> m_CameraProperties;
	};
}