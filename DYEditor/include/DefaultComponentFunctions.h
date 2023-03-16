#pragma once

#include "Entity.h"

using namespace DYE::DYEntity;

namespace DYE::DYEditor
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

	template<typename T>
	void DefaultRemoveComponentOfType(Entity& entity)
	{
		entity.RemoveComponent<T>();
	}
}