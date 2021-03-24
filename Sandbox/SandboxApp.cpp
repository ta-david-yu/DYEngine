#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/Entity.h"
#include "Scene/SceneLayer.h"
#include "Scene/Transform.h"
#include "Util/Type.h"

#include "SandboxLayer.h"
#include "FrameCounterComponent.h"

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
        }

        ~SandboxApp() final
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
