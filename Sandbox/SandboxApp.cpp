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

        ///
        std::shared_ptr<ShaderProgram> m_DebugShaderProgram;
        std::shared_ptr<VertexArray> m_DebugVA;

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
            auto sceneLayer = std::make_shared<SceneLayer>(m_Window.get());
            pushLayer(sceneLayer);

            /// Init sorting layer
            auto imgRendererUpdater = sceneLayer->GetComponentUpdaterOfType<ImageRenderer, ImageRendererUpdater>();
            imgRendererUpdater->PushSortingLayer("TEST 00");

            /// Create entities and components
            auto emptyEnt = sceneLayer->CreateEntity("Empty Ent");
            sceneLayer->LazyAddComponentToEntity<ImageRenderer>(emptyEnt);

            auto [hasTransform, _] = emptyEnt.lock()->GetComponent<Transform>();

            for (int i = 0; i < 2; i++)
            {
                auto frameCounterEnt = sceneLayer->CreateEntity("Frame Counter Ent " + std::to_string(i));
                sceneLayer->LazyAddComponentToEntity<ImageRenderer>(frameCounterEnt);
                if (i % 2 == 0)
                {
                    sceneLayer->LazyAddComponentToEntity<FrameCounterComponent>(frameCounterEnt);
                    sceneLayer->LazyAddComponentToEntity<FixedFrameCounterComponent>(frameCounterEnt);
                }
                else
                {
                    sceneLayer->LazyAddComponentToEntity<Subclass_FrameCounterComponent>(frameCounterEnt);
                    sceneLayer->LazyAddComponentToEntity<FixedFrameCounterComponent>(frameCounterEnt);
                }

                for (int childId = 0; childId < i*2; childId++)
                {
                    auto childEnt = sceneLayer->CreateEntity("Child " + std::to_string(childId));
                    childEnt.lock()->GetTransform().lock()->SetParent(frameCounterEnt.lock()->GetTransform());

                    for (int grandChildId = 0; grandChildId < childId; grandChildId++)
                    {
                        auto grandChildEnt = sceneLayer->CreateEntity("Grand Child " + std::to_string(grandChildId));
                        grandChildEnt.lock()->GetTransform().lock()->SetParent(childEnt.lock()->GetTransform());
                    }
                }
            }

            // Vertex Buffer

            /// Create vertices [position, color]
            float positions[4 * 6] = {
                    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    -0.25f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f,

                    -0.25f, -0.25f,   0.0f, 0.0f, 1.0f, 1.0f,
                    -1.0f, -0.25f,  0.0f, 0.0f, 0.0f, 1.0f
            };

            // Index Buffer
            std::uint32_t indices[] = {
                    0, 1, 2,
                    2, 3, 0
            };

            m_DebugVA = VertexArray::Create();

            auto vB = VertexBuffer::Create(positions, sizeof(positions));
            BufferLayout layout {
                BufferElement(ShaderDataType::Float2, "position", false),
                BufferElement(ShaderDataType::Float4, "color", false),
            };
            vB->SetLayout(layout);
            m_DebugVA->AddVertexBuffer(vB);

            auto iB = IndexBuffer::Create(indices, sizeof(indices) / sizeof(std::uint32_t));
            m_DebugVA->SetIndexBuffer(iB);

            /// Create debug shader program
            m_DebugShaderProgram = ShaderProgram::CreateFromFile("Basic", "assets/shaders/Basic.shader");

            m_DebugShaderProgram->Unbind();
            m_DebugVA->Unbind();
            vB->Unbind();
            iB->Unbind();

        }

        ~SandboxApp() final = default;

        void onPostRenderLayers() override
        {
            static float colorComponent = 1;
            colorComponent += TIME.DeltaTime() * 0.5f;
            if (colorComponent > 1)
                colorComponent -= 1;

            ///
            m_DebugShaderProgram->Bind();
            {
                unsigned int colorUniformLocation = glGetUniformLocation(m_DebugShaderProgram->GetID(), "_Color");
                glCheckAfterCall(glGetUniformLocation(m_DebugShaderProgram->GetID(), "_Color"));
                glCall(glUniform4f(colorUniformLocation, colorComponent, colorComponent, colorComponent,
                                   colorComponent));
            }

            ///
            m_DebugVA->Bind();
            RenderCommand::DrawIndexed(m_DebugVA);

            m_DebugShaderProgram->Unbind();
            m_DebugVA->Unbind();
        }
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new SandboxApp { "Sandbox", 60 };
}
