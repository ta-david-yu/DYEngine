#pragma once

namespace DYE::DYEditor
{
	class Scene;

	struct RuntimeSceneManagement
	{
		friend class SceneRuntimeLayer;
	public:
		static Scene& GetActiveMainScene();
	private:
		static void executeSceneOperationIfAny();
	};
}