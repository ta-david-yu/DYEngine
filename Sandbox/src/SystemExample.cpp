#include "SystemExample.h"

#include "World.h"

namespace SystemNamespace
{
	void UpdateMovementSystemExample(DYE::DYEntity::World &world)
	{

	}
}

void InitializeVelocitySystemExample(DYE::DYEntity::World &world)
{

}

void DerivedSystem::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	frameCounter++;
}
