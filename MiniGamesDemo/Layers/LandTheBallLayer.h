#pragma once

#include "Core/LayerBase.h"

#include "ColliderManager.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include "Objects/Camera.h"
#include "Objects/SpriteButton.h"
#include "Objects/SpriteUnsignedNumber.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace DYE
{
	class WindowBase;

	class LandTheBallLayer : public LayerBase
	{
	public:
		explicit LandTheBallLayer(Application& application);
		LandTheBallLayer() = delete;
		LandTheBallLayer(LandTheBallLayer const& other) = delete;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnFixedUpdate() override;
		void OnRender() override;
		void OnImGui() override;

	private:
		void registerBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		void unregisterBoxCollider(MiniGame::Transform& transform, MiniGame::BoxCollider& collider);
		void renderSprite(MiniGame::Transform& transform, MiniGame::Sprite& sprite);

		void debugInput();

	private:
		Application& m_Application;

		// Debug settings
		WindowBase* m_MainWindow = nullptr;
		bool m_DrawImGui = false;

		// Animation state/settings
		float m_BackgroundScrollingSpeed = 0.5f;

		// Game world
		ColliderManager m_ColliderManager;
		MiniGame::Camera m_MainCamera;

		MiniGame::Transform m_BackgroundTransform;
		MiniGame::Sprite m_BackgroundSprite;

		MiniGame::SpriteUnsignedNumber m_Number;
	};
}