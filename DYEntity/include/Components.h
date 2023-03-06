#pragma once

#include "TypeRegistry.h"
#include "Entity.h"

// Include all built-in components here...
#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"

namespace DYE::DYEntity
{
	struct _TestFloatComponent
	{
		float Value;
	};

	struct _TestIntComponent
	{
		int Value;
	};

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

	template<typename T>
	void DefaultRemoveComponentOfType(Entity& entity)
	{
		entity.RemoveComponent<T>();
	}

	void RegisterBuiltInComponentTypes();

	// TODO: RegistrBuiltInSystemTypes();
}
