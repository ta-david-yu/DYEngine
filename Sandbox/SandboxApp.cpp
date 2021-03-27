#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/Entity.h"
#include "Scene/SceneLayer.h"
#include "Scene/Transform.h"
#include "Util/Type.h"

#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"

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

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
            auto sceneLayer = std::make_shared<SceneLayer>(m_Window.get());
            pushLayer(sceneLayer);

            /// Manually setup component updaters (manual order)
            /*
            auto frameCounterUpdater = sceneLayer->CreateAndRegisterGenericComponentUpdater(
                    std::type_index(typeid(FrameCounterComponent)));

            auto fixedFrameCounterUpdater = sceneLayer->CreateAndRegisterGenericComponentUpdater(
                    std::type_index(typeid(FixedFrameCounterComponent)));
            */
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


                /*
                auto [hasComp, comp] = frameCounterEnt.lock()->GetComponent<Subclass_FrameCounterComponent>();
                if (hasComp)
                {
                    SDL_Log("%s: %s", frameCounterEnt.lock()->GetName().c_str(), demangleCTypeName(typeid(*comp.lock()).name()).c_str());
                }

                frameCounterUpdater.lock()->AttachEntityWithComponent(
                        frameCounterEnt,
                        new FrameCounterComponent());
                fixedFrameCounterUpdater.lock()->AttachEntityWithComponent(
                        frameCounterEnt,
                        new FixedFrameCounterComponent());
                */
            }

            /// Create vertices
            // Vertex Buffer
            glm::vec2 positions[] = {
                    glm::vec2{-0.5f, -0.5f},
                    glm::vec2{0.5f, -0.5f},
                    //glm::vec2{0.5f, 0.5f},

                    glm::vec2{0.5f, 0.5f},
                    glm::vec2{-0.5f, 0.5f},
                    //glm::vec2{-0.5f, -0.5f}
            };

            // Index Buffer
            unsigned int indices[] = {
                    0, 1, 2,
                    2, 3, 0
            };

            unsigned int vertexBufferId;
            glGenBuffers(1, &vertexBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), positions, GL_STATIC_DRAW);

            // location (index), count (pos2d now), type (float), stride (the size of the struct), the local location pointer to the attribute (null in our case)
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

            unsigned int indexBufferId;
            glGenBuffers(1, &indexBufferId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

            /// Create debug shader program
            m_DebugShaderProgram = ShaderProgram::CreateFromFile("Basic", "assets/shaders/Basic.shader");
            m_DebugShaderProgram->Bind();
        }

        ~SandboxApp() final = default;

        void onPostRenderLayers() override
        {
            static float blueComponent = 0;
            blueComponent += TIME.DeltaTime();
            if (blueComponent > 1)
                blueComponent -= 1;

            unsigned int colorUniformLocation = glGetUniformLocation(m_DebugShaderProgram->GetID(), "_Color");
            glCall(glUniform4f(colorUniformLocation, 0.2, 0.2, blueComponent, 0));

            //glDrawArrays(GL_TRIANGLES, 0, 6);
            // Primitive Type, NumOfIndices, Index Type, pointer to the indices (nullptr because we've already bound the IBO)
            glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        }
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new SandboxApp { "Sandbox", 60 };
}
