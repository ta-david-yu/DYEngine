#include "World.h"

#include "Entity.h"
#include "Components.h"

namespace DYE::DYEntity
{
	World::World()
	{
		// TEMP
		// TODO: remove this create entity call
		auto entity = CreateEntity("Ha Ha");
	}

	Entity World::CreateEntity()
	{
		return Entity(*this);
	}

	Entity World::CreateEntity(std::string const& name)
	{
		auto entity = Entity(*this);
		entity.AddComponent<NameComponent>(name);

		return entity;
	}
}