#pragma once

#include "Core/LayerBase.h"
#include "Event/KeyEvent.h"
#include "Util/FPSCounter.h"

#include "ColliderManager.h"
#include "Objects/PlayerPaddle.h"
#include "Objects/Wall.h"
#include "Objects/Ball.h"
#include "Objects/Camera.h"

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
		void registerBoxCollider(Pong::Transform& transform, Pong::BoxCollider& collider);
		void unregisterBoxCollider(Pong::Transform& transform, Pong::BoxCollider& collider);
		/// \return the velocity per second for the paddle.
		glm::vec3 updatePaddle(Pong::PlayerPaddle& paddle);
		void updateBoxCollider(Pong::Transform& transform, Pong::BoxCollider& collider);
		void renderSprite(Pong::Transform& transform, Pong::Sprite& sprite);

		void imguiSpriteObject(SpriteObject& object);

	private:
		bool m_IsControlWindowOpen = false;
		FPSCounter m_FPSCounter = FPSCounter(0.25);

		ColliderManager m_ColliderManager;
		std::shared_ptr<CameraProperties> m_CameraProperties;

		WindowBase* m_SecondWindow = nullptr;
		glm::vec2 m_WindowPosition;
		float m_TargetWindowWidth = 1600;
		float m_TargetWindowHeight = 900;
		float m_WindowPixelChangePerSecond = 300.0f;

		Pong::Camera m_MainCamera;
		Pong::Camera m_DebugCamera;
		Pong::Ball m_Ball;
		Pong::PlayerPaddle m_PlayerPaddle1;
		Pong::PlayerPaddle m_PlayerPaddle2;
		std::vector<Pong::Wall> m_Walls;

		Pong::Transform m_BackgroundTransform;
		Pong::Sprite m_BackgroundSprite;
	};
}