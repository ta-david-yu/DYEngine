#pragma once

#include "TypeRegistry.h"
#include "Entity.h"

// Include all built-in components here...
#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"

namespace DYE::DYEntity
{
	void RegisterBuiltInComponentTypes();

	// TODO: RegistrBuiltInSystemTypes();
}
