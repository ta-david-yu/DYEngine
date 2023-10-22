#include "Systems/ExecuteLoadSceneCommandSystem.h"

#include "Core/Entity.h"
#include "Components/Command/LoadSceneComponent.h"
#include "Core/RuntimeSceneManagement.h"
#include "FileSystem/FileSystem.h"
#include "Util/Logger.h"

namespace DYE::DYEditor
{
	void ExecuteLoadSceneCommandSystem::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
	{
		auto view = world.GetRegistry().view<LoadSceneComponent>();

		for (auto rawEntity : view)
		{
			auto loadSceneCommand = view.get<LoadSceneComponent>(rawEntity);

			if (!FileSystem::FileExists(loadSceneCommand.SceneAssetPath))
			{
				DYE_LOG("A load scene command is ignored because the given path '%s' doesn't exist.", loadSceneCommand.SceneAssetPath.string().c_str());
				continue;
			}

			DYE_LOG("Load Scene Command Executed: '%s'", loadSceneCommand.SceneAssetPath.string().c_str());
			RuntimeSceneManagement::LoadScene(loadSceneCommand.SceneAssetPath);

			// For now, we just break because there could only be one scene-loading operation at a time.
			break;
		}

		// Destroy all load scene command entities.
		world.GetRegistry().destroy(view.begin(), view.end());
	}
}
