#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/Entity.h"
#include "Scene/SceneLayer.h"
#include "Scene/Transform.h"
#include "Util/Type.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Texture.h"

#include "Scene/ImageRenderer.h"
#include "Scene/ImagePointerEventHandler.h"
#include "SandboxLayer.h"
#include "FrameCounterComponent.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace DYE
{
    std::vector<std::weak_ptr<ImageRenderer>> g_Images;
    int g_CurrentImageIndex {0};

    void PressRightButton(const ImagePointerEventHandler& handler)
    {
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(false);
        g_CurrentImageIndex++;
        if (g_CurrentImageIndex > g_Images.size() - 1)
            g_CurrentImageIndex = 0;
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(true);
    }

    void PressLeftButton(const ImagePointerEventHandler& handler)
    {
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(false);
        g_CurrentImageIndex--;
        if (g_CurrentImageIndex < 0)
            g_CurrentImageIndex = g_Images.size() - 1;
        g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(true);
    }

    class SandboxApp final : public Application
    {
    public:
        SandboxApp() = delete;
        SandboxApp(const SandboxApp &) = delete;

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
            RenderCommand::SetClearColor(glm::vec4 {51, 63, 88, 255} / 255.0f);

            auto sceneLayer = std::make_shared<SceneLayer>(m_Window.get());
            pushLayer(sceneLayer);

            /// Init pointer event handler updater
            auto pointerEventHandlerUpdater =
                    std::make_shared<ImagePointerEventHandlerUpdater>(ComponentTypeID(typeid(ImagePointerEventHandler)), m_Window.get());
            sceneLayer->RegisterComponentUpdater(std::move(pointerEventHandlerUpdater));

            /// Init sorting layer
            auto imgRendererUpdater = sceneLayer->GetComponentUpdaterOfType<ImageRenderer, ImageRendererUpdater>();
            imgRendererUpdater->PushSortingLayer("TEST 00");

            auto rightBtn = sceneLayer->CreateEntity("Right Btn");
            rightBtn.lock()->GetTransform()->SetLocalPosition({1200, 450, 0});
            auto image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(rightBtn);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/Island.png"));
            image.lock()->SetSortingOrder(0);
            auto eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(rightBtn);
            eventHandler.lock()->SetImage(image);
            eventHandler.lock()->OnPointerUpCallback = PressRightButton;

            auto leftBtn = sceneLayer->CreateEntity("Right Btn");
            leftBtn.lock()->GetTransform()->SetLocalPosition({400, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(leftBtn);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/Island.png"));
            image.lock()->SetSortingOrder(0);
            eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(leftBtn);
            eventHandler.lock()->SetImage(image);
            eventHandler.lock()->OnPointerUpCallback = PressLeftButton;

            /// Create entities and components
            auto treeMoonCat = sceneLayer->CreateEntity("TreeMoonCat");
            treeMoonCat.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(treeMoonCat);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/TreeMoonCat.png"));
            image.lock()->SetSortingOrder(1);

            g_Images.push_back(image);


            auto stranded = sceneLayer->CreateEntity("Stranded");
            stranded.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(stranded);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/stranded-social-media.png"));
            image.lock()->SetSortingOrder(2);

            g_Images.push_back(image);


            auto peaceOut = sceneLayer->CreateEntity("Peace Out");
            peaceOut.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(peaceOut);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/peaceout-boy.png"));
            image.lock()->SetSortingOrder(3);

            g_Images.push_back(image);

            for (const auto& img : g_Images)
            {
                img.lock()->GetEntityPtr()->SetActive(false);
            }
            g_Images[g_CurrentImageIndex].lock()->GetEntityPtr()->SetActive(true);
        }

        ~SandboxApp() final = default;

        void onPostRenderLayers() override
        {
        }
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new SandboxApp { "Sandbox", 60 };
}
