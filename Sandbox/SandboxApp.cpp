#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/Entity.h"
#include "Scene/SceneLayer.h"
#include "Scene/Transform.h"
#include "Util/Type.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"

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
        std::shared_ptr<VertexBuffer> m_DebugVB;
        std::shared_ptr<IndexBuffer> m_DebugIB;
        unsigned int m_VAO;

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
            auto sceneLayer = std::make_shared<SceneLayer>(m_Window.get());
            pushLayer(sceneLayer);

            /// Create entities and components
            auto emptyEnt = sceneLayer->CreateEntity("Empty Ent");
            auto [hasTransform, _] = emptyEnt.lock()->GetComponent<Transform>();

            for (int i = 0; i < 4; i++)
            {
                auto frameCounterEnt = sceneLayer->CreateEntity("Frame Counter Ent " + std::to_string(i));
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
                    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
                    0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,

                    0.5f, 0.5f,   0.0f, 0.0f, 1.0f, 1.0f,
                    -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f
            };

            // Index Buffer
            std::uint32_t indices[] = {
                    0, 1, 2,
                    2, 3, 0
            };

            glGenVertexArrays(1, &m_VAO);
            glBindVertexArray(m_VAO);

            m_DebugVB = VertexBuffer::Create(positions, sizeof(positions));
            BufferLayout layout {
                BufferElement(ShaderDataType::Float2, "position", false),
                BufferElement(ShaderDataType::Float4, "color", false),
            };
            m_DebugVB->SetLayout(layout);

            uint32_t index = 0;
            for (const auto& element : m_DebugVB->GetLayout())
            {
                // location (index), count (pos2d now), type (float), stride (the size of the struct), the local location pointer to the attribute (null in our case)
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index,
                                      element.GetComponentCount(),
                                      ShaderDataTypeToOpenGLBaseType(element.Type),
                                      element.Normalized? GL_TRUE: GL_FALSE,
                                      layout.GetStride(),
                                      (const void*) element.Offset);
                index++;
            }

            m_DebugIB = IndexBuffer::Create(indices, sizeof(indices) / sizeof(std::uint32_t));

            /// Create debug shader program
            m_DebugShaderProgram = ShaderProgram::CreateFromFile("Basic", "assets/shaders/Basic.shader");

            glBindVertexArray(0);
            m_DebugShaderProgram->Unbind();
            m_DebugVB->Unbind();
            m_DebugIB->Unbind();
        }

        ~SandboxApp() final = default;

        void onPostRenderLayers() override
        {
            static float blueComponent = 0;
            blueComponent += TIME.DeltaTime() * 0.5f;
            if (blueComponent > 1)
                blueComponent -= 1;

            ///
            m_DebugShaderProgram->Bind();
            unsigned int colorUniformLocation = glGetUniformLocation(m_DebugShaderProgram->GetID(), "_Color");
            glCheckAfterCall(glGetUniformLocation(m_DebugShaderProgram->GetID(), "_Color"));

            glCall(glUniform4f(colorUniformLocation, 0.2, 0.2, blueComponent, 0));

            ///
            glBindVertexArray(m_VAO);

            ///
            m_DebugIB->Bind();
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

            // Primitive Type, NumOfIndices, Index Type, pointer to the indices (nullptr because we've already bound the IBO)
            glCall(glDrawElements(GL_TRIANGLES, m_DebugIB->GetCount(), GL_UNSIGNED_INT, nullptr));
        }
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new SandboxApp { "Sandbox", 60 };
}
