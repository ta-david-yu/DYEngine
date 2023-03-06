#pragma once

#include "TypeRegistry.h"
#include "Entity.h"
#include "ImGui/ImGuiUtil.h"

// Include all built-in components here...
#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"

namespace DYE::DYEntity
{
	template<typename T>
	bool DefaultHasComponentOfType(Entity& entity)
	{
		return entity.HasComponent<T>();
	}

	template<typename T>
	void DefaultAddComponentOfType(Entity& entity)
	{
		// By default, add component to the entity using default constructor
		entity.AddComponent<T>();
	}

	void RegisterBuiltInComponentTypes();

	// TODO: RegistrBuiltInSystemTypes();
}
