#pragma once

#include "EditorCore.h"

namespace DYE::DYEntity
{
	class World;
}

namespace SystemNamespace
{
	DYE_SYSTEM_FUNCTION("Initialize Velocity", SystemNamespace::UpdateMovementSystemExample)
	void UpdateMovementSystemExample(DYE::DYEntity::World& world);
}

DYE_SYSTEM_FUNCTION("Initialize Velocity", InitializeVelocitySystemExample)
void InitializeVelocitySystemExample(DYE::DYEntity::World& world);

DYE_SYSTEM_FUNCTION_SIGNATURE(ExampleFunction)
{

};

