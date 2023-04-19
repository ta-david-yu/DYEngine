#include "AppEntryPoint.h"
#include "Core/Application.h"

#include "SandboxLayer.h"

#include "SceneEditorLayer.h"
#include "SceneRuntimeLayer.h"


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
#ifdef DYE_RUNTIME
			auto runtimeLayer = std::make_shared<DYEditor::SceneRuntimeLayer>();
			pushLayerImmediate(runtimeLayer);
#endif

#ifdef DYE_EDITOR
			auto runtimeLayer = std::make_shared<DYEditor::SceneRuntimeLayer>();
			auto editorLayer = std::make_shared<DYEditor::SceneEditorLayer>();
			editorLayer->SetApplication(this);
			editorLayer->SetRuntimeLayer(runtimeLayer);

			pushLayerImmediate(runtimeLayer);
			pushLayerImmediate(editorLayer);
#endif
			//pushLayerImmediate(std::make_shared<SandboxLayer>());
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
