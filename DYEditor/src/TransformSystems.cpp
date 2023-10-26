#include "Systems/TransformSystems.h"

#include "Core/Entity.h"
#include "Components/TransformComponents.h"

namespace DYE::DYEditor
{

	void ComputeLocalToWorldSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		// TODO: use observer to iterate through entity with changed transform components in the last tick,
		//		 update LocalToWorld system.
	}
}