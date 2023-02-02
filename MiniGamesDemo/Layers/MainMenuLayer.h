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

	class MainMenuLayer : public LayerBase
	{
	public:
		explicit MainMenuLayer(Application& application);
		MainMenuLayer() = delete;
		MainMenuLayer(MainMenuLayer const& other) = delete;

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

		void onSelect(int index);
		void onConfirm(int index);

		void debugInput();
		void imguiSprite(const std::string& name, MiniGame::Transform& transform, MiniGame::Sprite& sprite);

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

		MiniGame::Transform m_LogoTransform;
		MiniGame::Sprite m_LogoSprite;

		MiniGame::Transform m_ButtonPromptTransform;
		MiniGame::Sprite m_ButtonPromptSprite;

		MiniGame::Transform m_SubtitleTransform;
		MiniGame::Sprite m_SubtitleSprite;
		std::shared_ptr<Texture2D> m_LandTheBallSubtitleTexture;
		std::shared_ptr<Texture2D> m_PongSubtitleTexture;
		std::shared_ptr<Texture2D> m_ExitSubtitleTexture;

		std::vector<MiniGame::SpriteButton> m_MenuButtons;
		int m_SelectedButtonIndex = 0;

		MiniGame::SpriteButton m_LandTheBallButton;
		MiniGame::SpriteButton m_PongButton;

		MiniGame::Transform m_HighScoreTextTransform;
		MiniGame::Sprite m_HighScoreTextSprite;
		MiniGame::SpriteUnsignedNumber m_LandTheBallHighScoreNumber;
	};
}
