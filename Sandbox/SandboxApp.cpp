#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/Entity.h"
#include "Scene/SceneLayer.h"
#include "Scene/Transform.h"
#include "Util/TypeUtil.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Texture.h"

#include "Scene/ImageRenderer.h"

#include "ImagePointerEventHandler.h"
#include "SandboxLayer.h"
#include "FrameCounterComponent.h"
#include "SubtitleUpdater.h"
#include "SandboxMessage.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <yaml-cpp/yaml.h>

namespace DYE
{
    std::weak_ptr<SubtitleUpdater> m_SubtitleUpdater;
    std::vector<std::weak_ptr<ImageRenderer>> g_Images;
    std::vector<std::string> g_Descriptions;
    int g_CurrentImageIndex {0};

    void ReleasedRightButton(const ImagePointerEventHandler& handler)
    {
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(false);
        g_CurrentImageIndex++;
        if (g_CurrentImageIndex > g_Images.size() - 1)
            g_CurrentImageIndex = 0;
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(true);
        m_SubtitleUpdater.lock()->Description = g_Descriptions[g_CurrentImageIndex];

        handler.GetImage().lock()->SetColor({1, 1, 1, 1});
    }

    void ReleasedLeftButton(const ImagePointerEventHandler& handler)
    {
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(false);
        g_CurrentImageIndex--;
        if (g_CurrentImageIndex < 0)
            g_CurrentImageIndex = (int)g_Images.size() - 1;
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(true);
        m_SubtitleUpdater.lock()->Description = g_Descriptions[g_CurrentImageIndex];

        handler.GetImage().lock()->SetColor({1, 1, 1, 1});
    }

    void PressedButton(const ImagePointerEventHandler& handler)
    {
        handler.GetImage().lock()->SetColor({0.8, 0.8, 0.8, 1});
    }

    void HandleOnPointerEnterButton(const ImagePointerEventHandler& handler)
    {
        handler.GetImage().lock()->SetColor({1, 1, 1, 1});
    }

    void HandleOnPointerExitButton(const ImagePointerEventHandler& handler)
    {
        handler.GetImage().lock()->SetColor({1, 1, 1, 0.5});
    }


    class SandboxApp final : public Application
    {
    public:
        SandboxApp() = delete;
        SandboxApp(const SandboxApp &) = delete;

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
			auto sandboxLayer = std::make_shared<SandboxLayer>(m_Window.get());
			pushLayer(sandboxLayer);
        }

        ~SandboxApp() final = default;

	private:
		/// Function made for CGL BA3 assignment (point n click game engine).
		/// Call this function in SandboxApp ctor to create the level.
		void createSceneAndPopulateGallery()
		{
			RenderCommand::GetInstance().SetClearColor(glm::vec4 {51, 63, 88, 255} / 255.0f);
			auto sceneLayer = std::make_shared<SceneLayer>(m_Window.get());
			pushLayer(sceneLayer);

			/// Init pointer event handler updater
			auto pointerEventHandlerUpdater =
					std::make_shared<ImagePointerEventHandlerUpdater>(ComponentTypeID(typeid(ImagePointerEventHandler)), m_Window.get());
			sceneLayer->RegisterComponentUpdater(std::move(pointerEventHandlerUpdater));

			auto subtitleUpdater =
					std::make_shared<SubtitleUpdater>(ComponentTypeID(typeid(SubtitleUpdater)));
			m_SubtitleUpdater = subtitleUpdater;
			sceneLayer->RegisterComponentUpdater(std::move(subtitleUpdater));

			/// Init sorting layer
			auto imgRendererUpdater = sceneLayer->GetComponentUpdaterOfType<ImageRenderer, ImageRendererUpdater>();
			imgRendererUpdater->PushSortingLayer("TEST 00");

			auto rightBtn = sceneLayer->CreateEntity("Right Btn");
			rightBtn.lock()->GetTransform()->SetLocalPosition({1100, 450, 0});
			auto image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(rightBtn);
			image.lock()->SetTexture(Texture2D::Create("assets/textures/right-arrow-inactive.png"));
			image.lock()->SetSortingOrder(0);
			image.lock()->SetColor({1, 1, 1, 0.5});
			auto eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(rightBtn);
			eventHandler.lock()->SetImage(image);
			eventHandler.lock()->OnPointerUpCallback = ReleasedRightButton;
			eventHandler.lock()->OnPointerDownCallback = PressedButton;
			eventHandler.lock()->OnPointerEnterCallback = HandleOnPointerEnterButton;
			eventHandler.lock()->OnPointerExitCallback = HandleOnPointerExitButton;

			auto leftBtn = sceneLayer->CreateEntity("Left Btn");
			leftBtn.lock()->GetTransform()->SetLocalPosition({500, 450, 0});
			image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(leftBtn);
			image.lock()->SetTexture(Texture2D::Create("assets/textures/left-arrow-inactive.png"));
			image.lock()->SetSortingOrder(0);
			image.lock()->SetColor({1, 1, 1, 0.5});
			eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(leftBtn);
			eventHandler.lock()->SetImage(image);
			eventHandler.lock()->OnPointerUpCallback = ReleasedLeftButton;
			eventHandler.lock()->OnPointerDownCallback = PressedButton;
			eventHandler.lock()->OnPointerEnterCallback = HandleOnPointerEnterButton;
			eventHandler.lock()->OnPointerExitCallback = HandleOnPointerExitButton;

			/// Loading Gallery
			YAML::Node rootNode = YAML::LoadFile("assets/gallery.yaml");
			if (rootNode)
			{
				const auto& galleryNode = rootNode["Gallery"];
				for (const auto& imageNode : galleryNode)
				{
					const auto& name = imageNode["Name"].as<std::string>();
					const auto& texturePath = imageNode["Texture"].as<std::string>();
					const auto& description = imageNode["Description"].as<std::string>();
					glm::vec2 position { imageNode["Position"][0].as<float>(), imageNode["Position"][1].as<float>() };

					DYE_LOG("Insert - Name: %s, Texture: %s, Description %s at [%f, %f]",
							name.c_str(),
							texturePath.c_str(),
							description.c_str(),
							position[0],
							position[1]);

					auto newEntry = sceneLayer->CreateEntity(name);
					newEntry.lock()->GetTransform()->SetLocalPosition({position.x, position.y, 0});
					image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(newEntry);
					image.lock()->SetTexture(Texture2D::Create(texturePath));
					image.lock()->SetSortingOrder(0);

					g_Images.push_back(image);
					g_Descriptions.emplace_back(description);
				}
			}
			else
			{
				DYE_LOG("Failed to load the yaml file");
			}

			for (const auto& img : g_Images)
			{
				img.lock()->GetEntityPtr()->SetActive(false);
			}
			g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(true);
			m_SubtitleUpdater.lock()->Description = g_Descriptions[g_CurrentImageIndex];
		}
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new SandboxApp { "Sandbox", 60 };
}
