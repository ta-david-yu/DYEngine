#include "DYEditorApplication.h"

#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"
#include "ProjectConfig.h"
#include "Core/RuntimeSceneManagement.h"

namespace DYE::DYEditor
{
	DYEditorApplication::DYEditorApplication(WindowProperties mainWindowProperties, int fixedFramePerSecond)
		: Application(mainWindowProperties, fixedFramePerSecond)
	{
		DYE::DYEditor::RegisterBuiltInTypes();
		DYE::DYEditor::RegisterUserTypes();

#ifdef DYE_RUNTIME
		auto runtimeLayer = std::make_shared<DYEditor::SceneRuntimeLayer>();
		pushLayerImmediate(runtimeLayer);


		std::filesystem::path firstScenePath = (std::filesystem::path) DYE::DYEditor::GetRuntimeConfig().GetOrDefault<std::string>(DYE::DYEditor::RuntimeConfigFirstSceneKey, "");
		if (firstScenePath.empty())
		{
			DYE_ASSERT_RELEASE(false && "The settings for the first scene is empty."
										"Be sure to set Project.FirstScene in the runtime configuration file (i.e. runtime.ini).");
		}
		DYE::DYEditor::RuntimeSceneManagement::LoadScene(firstScenePath);
#endif

#ifdef DYE_EDITOR
		auto runtimeLayer = std::make_shared<DYEditor::SceneRuntimeLayer>();
		auto editorLayer = std::make_shared<DYEditor::SceneEditorLayer>();
		editorLayer->SetApplication(this);
		editorLayer->SetRuntimeLayer(runtimeLayer);

		pushLayerImmediate(runtimeLayer);
		pushLayerImmediate(editorLayer);
#endif
	}
}