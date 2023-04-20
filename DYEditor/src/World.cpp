#include "../include/Core/World.h"

#include "../include/Core/Entity.h"
#include "../include/Components/NameComponent.h"

namespace DYE::DYEditor
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

	entt::registry& GetWorldUnderlyingRegistry(World &world)
	{
		return world.m_Registry;
	}
}