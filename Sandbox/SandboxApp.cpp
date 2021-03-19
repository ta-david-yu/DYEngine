#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/SceneLayer.h"

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

            /// Manully setup component updaters (manual order)
            /*
            auto frameCounterUpdater = sceneLayer->CreateAndRegisterGenericComponentUpdater(
                    std::type_index(typeid(FrameCounterComponent)));

            auto fixedFrameCounterUpdater = sceneLayer->CreateAndRegisterGenericComponentUpdater(
                    std::type_index(typeid(FixedFrameCounterComponent)));
            */
            /// Create 3 entities and components
            for (int i = 0; i < 3; i++)
            {
                auto frameCounterEnt = sceneLayer->CreateEntity("Frame Counter Ent");

                sceneLayer->LazyAddComponentToEntity<FrameCounterComponent>(frameCounterEnt);
                sceneLayer->LazyAddComponentToEntity<FixedFrameCounterComponent>(frameCounterEnt);

                /*
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
