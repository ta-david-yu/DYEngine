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
#include "SandboxLayer.h"
#include "FrameCounterComponent.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace DYE
{
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

            /// Init sorting layer
            auto imgRendererUpdater = sceneLayer->GetComponentUpdaterOfType<ImageRenderer, ImageRendererUpdater>();
            imgRendererUpdater->PushSortingLayer("TEST 00");

            /// Create entities and components
            auto island = sceneLayer->CreateEntity("Island");
            auto image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(island);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/Island.png"));

            auto treeMoonCat = sceneLayer->CreateEntity("TreeMoonCat");
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(treeMoonCat);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/TreeMoonCat.png"));

            auto stranded = sceneLayer->CreateEntity("Stranded");
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(stranded);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/stranded-social-media.png"));

            auto peaceOut = sceneLayer->CreateEntity("Peace Out");
            image = sceneLayer->LazyAddComponentToEntity<ImageRenderer>(peaceOut);
            image.lock()->SetTexture(Texture2D::Create("assets/textures/peaceout-boy.png"));
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
