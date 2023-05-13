#include "AppEntryPoint.h"
#include "Core/Application.h"

#include "DYEditorApplication.h"
#include "SandboxLayer.h"
#include "Configuration/ProjectConfig.h"

namespace DYE::Sandbox
{
    class SandboxApp final : public Application
    {
    public:
        SandboxApp() = delete;
        SandboxApp(const SandboxApp &) = delete;

        explicit SandboxApp(WindowProperties mainWindowProperties, int fixedFramePerSecond = 60)
            : Application(mainWindowProperties, fixedFramePerSecond)
        {
			pushLayerImmediate(std::make_shared<SandboxLayer>());
        }

        ~SandboxApp() final = default;
    };
}

using namespace DYE::DYEditor;

/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    //return new Sandbox::SandboxApp { "Sandbox", 60 };
	ProjectConfig &runtimeConfig = DYEditor::GetRuntimeConfig();
	auto projectName = runtimeConfig.GetOrDefault<std::string>(RuntimeConfigKeys::ProjectName, "Sandbox");
	auto mainWindowWidth = runtimeConfig.GetOrDefault<int>(RuntimeConfigKeys::MainWindowWidth, 1600);
	auto mainWindowHeight = runtimeConfig.GetOrDefault<int>(RuntimeConfigKeys::MainWindowHeight, 900);

	return new DYEditorApplication
	{
		WindowProperties
		{
			projectName,
			(std::uint32_t) mainWindowWidth,
			(std::uint32_t) mainWindowHeight
		},
		60
	};
}
