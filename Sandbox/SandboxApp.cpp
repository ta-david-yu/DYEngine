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
    void SetImageColor(const ImagePointerEventHandler& handler)
    {
        handler.GetImage().lock()->SetColor(glm::vec4{1, 1, 1, 0.5f});
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

            /// Create entities and components
            auto island = sceneLayer->CreateEntity("Island");
            island.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            auto image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(island);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/Island.png"));
            image.lock()->SetSortingOrder(0);
            auto eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(island);
            eventHandler.lock()->SetImage(image);
            eventHandler.lock()->OnPointerDownCallback = SetImageColor;


            auto treeMoonCat = sceneLayer->CreateEntity("TreeMoonCat");
            treeMoonCat.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(treeMoonCat);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/TreeMoonCat.png"));
            image.lock()->SetSortingOrder(1);
            eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(treeMoonCat);
            eventHandler.lock()->SetImage(image);
            eventHandler.lock()->OnPointerDownCallback = SetImageColor;


            auto stranded = sceneLayer->CreateEntity("Stranded");
            stranded.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(stranded);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/stranded-social-media.png"));
            image.lock()->SetSortingOrder(2);
            eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(stranded);
            eventHandler.lock()->SetImage(image);
            eventHandler.lock()->OnPointerDownCallback = SetImageColor;


            auto peaceOut = sceneLayer->CreateEntity("Peace Out");
            peaceOut.lock()->GetTransform()->SetLocalPosition({800, 450, 0});
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(peaceOut);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/peaceout-boy.png"));
            image.lock()->SetSortingOrder(3);
            eventHandler = sceneLayer->LazyAddComponentToEntity<ImagePointerEventHandler>(peaceOut);
            eventHandler.lock()->SetImage(image);
            eventHandler.lock()->OnPointerDownCallback = SetImageColor;
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
