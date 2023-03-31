#pragma once

#include "Entity.h"

namespace DYE::DYEditor
{
	template<typename T>
	bool DefaultHasComponentOfType(DYE::DYEntity::Entity& entity)
	{
		return entity.HasComponent<T>();
	}

	template<typename T>
	void DefaultAddComponentOfType(DYE::DYEntity::Entity& entity)
	{
		// By default, add component to the entity using default constructor
		entity.AddComponent<T>();
	}

	template<typename T>
	void DefaultRemoveComponentOfType(DYE::DYEntity::Entity& entity)
	{
		entity.RemoveComponent<T>();
	}
}