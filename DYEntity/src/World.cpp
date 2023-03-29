#include "World.h"

#include "Entity.h"
#include "Components.h"

namespace DYE::DYEntity
{
	World::World()
	{
	}

	Entity World::CreateEntity()
	{
		return Entity(*this, m_Registry.create());
	}

	Entity World::CreateEntity(std::string const& name)
	{
		auto entity = Entity(*this, m_Registry.create());
		entity.AddComponent<NameComponent>(name);

		return entity;
	}

	void World::DestroyEntity(Entity &entity)
	{
		m_Registry.destroy(entity.m_EntityHandle);
	}
}