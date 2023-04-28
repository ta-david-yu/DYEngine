#include "Components/HierarchyComponents.h"

#include "Core/Entity.h"
#include "Core/World.h"

namespace DYE::DYEditor
{
	Entity ParentComponent::Get(World &world) const
	{
		auto tryGetEntity = world.TryGetEntityWithGUID(ParentGUID);
		if (!tryGetEntity.has_value())
		{
			DYE_LOG("ParentComponent::Get: Cannot find the parent entity with GUID '%s'.", ParentGUID.ToString().c_str());
			return {};
		}

		return tryGetEntity.value();
	}
}