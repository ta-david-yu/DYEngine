#include "Core/World.h"

#include "Core/Entity.h"
#include "Components/IDComponent.h"
#include "Components/NameComponent.h"

#include <algorithm>

namespace DYE::DYEditor
{
	World::World()
	{
	}

	Entity World::CreateEntity(std::string const& name)
	{
		auto entity = Entity(*this, m_Registry.create());
		auto guid = m_EntityGUIDFactory.Generate();
		entity.AddComponent<IDComponent>().ID = guid;
		entity.AddComponent<NameComponent>(name);

		m_EntityHandles.push_back(EntityHandle { .Identifier = entity.m_EntityIdentifier });
		m_GUIDToEntityIdentifierMap.insert({guid, entity.m_EntityIdentifier});

		return entity;
	}

	Entity World::CreateEntityAtIndex(const std::string &name, std::size_t index)
	{
		auto entity = Entity(*this, m_Registry.create());
		auto guid = m_EntityGUIDFactory.Generate();
		entity.AddComponent<IDComponent>().ID = guid;
		entity.AddComponent<NameComponent>(name);

		m_EntityHandles.insert(m_EntityHandles.begin() + index, EntityHandle { .Identifier = entity.m_EntityIdentifier });
		m_GUIDToEntityIdentifierMap.insert({guid, entity.m_EntityIdentifier});

		return entity;
	}

	Entity World::CreateEntityWithGUID(std::string const& name, GUID guid)
	{
		auto entity = Entity(*this, m_Registry.create());
		entity.AddComponent<IDComponent>().ID = guid;
		entity.AddComponent<NameComponent>(name);

		m_EntityHandles.push_back(EntityHandle { .Identifier = entity.m_EntityIdentifier });
		m_GUIDToEntityIdentifierMap.insert({guid, entity.m_EntityIdentifier});

		return entity;
	}

	Entity World::WrapIdentifierIntoEntity(EntityIdentifier identifier)
	{
		return Entity(*this, identifier);
	}

	void World::DestroyEntity(Entity entity)
	{
		auto identifier = entity.m_EntityIdentifier;

		auto tryGetGUID = entity.TryGetGUID();
		if (tryGetGUID.has_value())
		{
			// Remove it from the GUID map if it has a GUID/IDComponent.
			m_GUIDToEntityIdentifierMap.erase(tryGetGUID.value());
		}

		// Remove the identifier from the handles array.
		auto newEnd = std::remove_if(m_EntityHandles.begin(), m_EntityHandles.end(),
									 [identifier](EntityHandle &element)
									 {
										 return element.Identifier == identifier;
									 });
		m_EntityHandles.erase(newEnd, m_EntityHandles.end());

		// Remove the entity from the actual world registry.
		m_Registry.destroy(identifier);
	}

	void World::DestroyEntityWithGUID(GUID entityGUID)
	{
		auto findItr = m_GUIDToEntityIdentifierMap.find(entityGUID);
		if (findItr == m_GUIDToEntityIdentifierMap.end())
		{
			// There is no entity with the given GUID recorded in the map, skip the function.
			return;
		}

		auto identifier = findItr->second;

		// Remove it from the GUID map.
		m_GUIDToEntityIdentifierMap.erase(findItr);

		// Remove the identifier from the handles array.
		auto newEnd = std::remove_if(m_EntityHandles.begin(), m_EntityHandles.end(),
									 [identifier](EntityHandle &element)
									 {
										 return element.Identifier == identifier;
									 });
		m_EntityHandles.erase(newEnd, m_EntityHandles.end());

		// Remove the entity from the actual world registry.
		m_Registry.destroy(identifier);
	}

	std::optional<Entity> World::TryGetEntityWithGUID(GUID entityGUID)
	{
		auto findItr = m_GUIDToEntityIdentifierMap.find(entityGUID);
		if (findItr == m_GUIDToEntityIdentifierMap.end())
		{
			// There is no entity with the given GUID recorded in the map.
			return {};
		}

		return Entity(*this, findItr->second);
	}

	std::optional<std::size_t> World::TryGetEntityIndex(Entity const &entity)
	{
		if (entity.m_pWorld != this)
		{
			return {};
		}

		for (int i = 0; i < m_EntityHandles.size(); ++i)
		{
			auto& entityHandle = m_EntityHandles[i];

			if (entityHandle.Identifier == entity.GetIdentifier())
			{
				return i;
			}
		}

		return {};
	}

	Entity World::GetEntityAtIndex(std::size_t index)
	{
		DYE_ASSERT(index < m_EntityHandles.size());
		return Entity(*this, m_EntityHandles[index].Identifier);
	}

	bool World::IsEmpty() const
	{
		return m_Registry.empty();
	}

	void World::Reserve(std::size_t size)
	{
		m_EntityHandles.reserve(size);
		m_GUIDToEntityIdentifierMap.reserve(size);
		m_Registry.reserve(size);
	}

	void World::Clear()
	{
		m_EntityHandles.clear();
		m_GUIDToEntityIdentifierMap.clear();
		m_Registry.clear<>();
	}

	Entity World::createUntrackedEntity()
	{
		return Entity(*this, m_Registry.create());
	}

	void World::registerUntrackedEntityAtIndex(Entity entity, std::size_t index)
	{
		m_EntityHandles.insert(m_EntityHandles.begin() + index, EntityHandle(entity.m_EntityIdentifier));

		auto tryGetGUID = entity.TryGetGUID();
		DYE_ASSERT_LOG_WARN(tryGetGUID.has_value(), "The given entity doesn't have a GUID (IDComponent), cannot be tracked.");

		DYE_ASSERT_LOG_WARN(!m_GUIDToEntityIdentifierMap.contains(tryGetGUID.value()), "The given entity's GUID has already been tracked in the map.");

		m_GUIDToEntityIdentifierMap.insert({tryGetGUID.value(), entity.m_EntityIdentifier});
	}

	entt::registry& GetWorldUnderlyingRegistry(World &world)
	{
		return world.m_Registry;
	}
}