#include "AppEntryPoint.h"
#include "Core/Application.h"

#include "SandboxLayer.h"

#include "EntityLevelEditorLayer.h"

namespace DYE::Sandbox
{
    class SandboxApp final : public Application
    {
    public:
        SandboxApp() = delete;
        SandboxApp(const SandboxApp &) = delete;

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
			pushLayerImmediate(std::make_shared<SandboxLayer>());
			pushLayerImmediate(std::make_shared<DYEditor::EntityLevelEditorLayer>());
        }

        ~SandboxApp() final = default;
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new Sandbox::SandboxApp { "Sandbox", 60 };
}
