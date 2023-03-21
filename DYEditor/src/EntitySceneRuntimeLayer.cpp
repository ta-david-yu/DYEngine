#include "../include/EntitySceneRuntimeLayer.h"

namespace DYE::DYEntity
{
	EntitySceneRuntimeLayer::EntitySceneRuntimeLayer() : DYE::LayerBase("Runtime")
	{

	}

	void EntitySceneRuntimeLayer::OnAttach()
	{
		onPreSceneFileLoaded();

		// TODO: populate the world with the entities listed in the level file

		onPostSceneFileLoaded();
	}
}