#include "SystemExample.h"

#include "World.h"

namespace SystemNamespace
{
	void DerivedSystemB::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
	{

	}
}

void DerivedSystemA::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	frameCounter++;
}

void DerivedSystemA::DrawInspector(DYE::DYEntity::World &world)
{
	SystemBase::DrawInspector(world);
}

