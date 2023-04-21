#include "Core/World.h"

#include "Core/Entity.h"
#include "Components/IDComponent.h"
#include "Components/NameComponent.h"

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
		return CreateEntityWithGUID(name, m_EntityGUIDFactory.Generate());
	}

	Entity World::CreateEntityWithGUID(std::string const& name, GUID guid)
	{
		auto entity = Entity(*this, m_Registry.create());
		entity.AddComponent<IDComponent>().ID = guid;
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