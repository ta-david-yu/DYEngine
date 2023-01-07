#pragma once

#include "Core/LayerBase.h"
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

    class PeepholismLayer : public LayerBase
    {
    public:
        PeepholismLayer();
		PeepholismLayer(PeepholismLayer const& other) = delete;

		void OnInit() override;
        void OnEvent(Event& event) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
		void OnRender() override;
        void OnImGui() override;

	private:
		static void renderSpriteObject(SpriteObject& object);
		static void renderTiledSpriteObject(SpriteObject& object, glm::vec2 offset);
		void imguiSpriteObject(SpriteObject& object);

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

		float m_TileOffset = 0.0f;

		std::shared_ptr<SpriteObject> m_OriginObject;
		std::shared_ptr<SpriteObject> m_AverageObject;
		std::shared_ptr<SpriteObject> m_MovingObject;
		std::shared_ptr<SpriteObject> m_BackgroundTileObject;

		glm::vec3 m_CollisionCenter {3, 0, 0};
		glm::vec3 m_CollisionSize {1, 1, 1};

		std::shared_ptr<CameraProperties> m_CameraProperties;
	};
}