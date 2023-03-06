#include "../include/EntityLevelRuntimeLayer.h"

namespace DYE::DYEntity
{
	EntityLevelRuntimeLayer::EntityLevelRuntimeLayer() : DYE::LayerBase("Runtime")
	{

	}

	void EntityLevelRuntimeLayer::OnAttach()
	{
		onPreLevelFileLoaded();

		// TODO: populate the world with the entities listed in the level file

		onPostLevelFileLoaded();
	}
}