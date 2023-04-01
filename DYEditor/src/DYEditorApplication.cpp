#include "DYEditorApplication.h"

#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"

namespace DYE::DYEditor
{
	DYEditorApplication::DYEditorApplication(const std::string &windowName, int fixedFramePerSecond)
		: Application(windowName, fixedFramePerSecond)
	{
//		DYEditor::RegisterBuiltInTypes();
//		DYEditor::RegisterUserTypes();

		auto runtimeLayer = std::make_shared<DYEditor::SceneRuntimeLayer>();
		auto editorLayer = std::make_shared<DYEditor::SceneEditorLayer>();
		editorLayer->SetRuntimeLayer(runtimeLayer);

		pushLayerImmediate(runtimeLayer);
		pushLayerImmediate(editorLayer);
	}
}