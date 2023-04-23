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
		auto entity = Entity(*this, m_Registry.create());

		m_EntityHandles.push_back(EntityHandle { .Identifier = entity.m_EntityIdentifier });

		return entity;
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

		m_EntityHandles.push_back(EntityHandle { .Identifier = entity.m_EntityIdentifier });

		return entity;
	}

	void World::DestroyEntity(Entity &entity)
	{
		EntityIdentifier identifier = entity.m_EntityIdentifier;
		auto newEnd = std::remove_if(m_EntityHandles.begin(), m_EntityHandles.end(),
					   [identifier](EntityHandle &element)
					   {
						   return element.Identifier == identifier;
					   });
		m_EntityHandles.erase(newEnd, m_EntityHandles.end());

		m_Registry.destroy(identifier);
	}

	void World::DestroyEntity(EntityIdentifier identifier)
	{
		std::remove_if(m_EntityHandles.begin(), m_EntityHandles.end(),
					   [identifier](EntityHandle &element)
					   {
						   return element.Identifier == identifier;
					   });

		m_Registry.destroy(identifier);
	}

	bool World::IsEmpty() const
	{
		return m_Registry.empty();
	}

	void World::Reserve(std::size_t size)
	{
		m_Registry.reserve(size);
		m_EntityHandles.reserve(size);
	}

	void World::Clear()
	{
		m_Registry.clear<>();
		m_EntityHandles.clear();
	}

	entt::registry& GetWorldUnderlyingRegistry(World &world)
	{
		return world.m_Registry;
	}
}