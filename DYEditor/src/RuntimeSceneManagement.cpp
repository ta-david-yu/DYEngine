#include "Core/RuntimeSceneManagement.h"

#include "Core/Scene.h"

namespace DYE::DYEditor
{
	struct RuntimeSceneManagementData
	{
		Scene ActiveMainScene;
	};

	static RuntimeSceneManagementData s_Data;

	Scene &RuntimeSceneManagement::GetActiveMainScene()
	{
		return s_Data.ActiveMainScene;
	}

	void RuntimeSceneManagement::executeSceneOperationIfAny()
	{
		// TODO: load any new scene OR unload scene setup etc.
	}
}