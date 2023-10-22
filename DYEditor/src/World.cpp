#include "Core/World.h"

#include "Core/Entity.h"
#include "Util/EntityUtil.h"
#include "Components/IDComponent.h"
#include "Components/NameComponent.h"
#include "Serialization/SerializedObjectFactory.h"

#include <algorithm>

namespace DYE::DYEditor
{
	World::World()
	{
	}

	Entity World::CreateCommandEntity()
	{
		auto entity = Entity(*this, m_Registry.create());
		return entity;
	}

	Entity World::CreateEntity(std::string const& name)
	{
		auto entity = Entity(*this, m_Registry.create());
		auto guid = m_EntityGUIDFactory.Generate();
		entity.AddComponent<IDComponent>().ID = guid;
		entity.AddComponent<NameComponent>(name);

#ifdef DYE_EDITOR
		entity.AddComponent<EntityEditorOnlyMetadata>();
#endif

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

#ifdef DYE_EDITOR
		entity.AddComponent<EntityEditorOnlyMetadata>();
#endif

		m_EntityHandles.insert(m_EntityHandles.begin() + index, EntityHandle { .Identifier = entity.m_EntityIdentifier });
		m_GUIDToEntityIdentifierMap.insert({guid, entity.m_EntityIdentifier});

		return entity;
	}

	Entity World::CreateEntityWithGUID(std::string const& name, GUID guid)
	{
		auto entity = Entity(*this, m_Registry.create());
		entity.AddComponent<IDComponent>().ID = guid;
		entity.AddComponent<NameComponent>(name);

#ifdef DYE_EDITOR
		entity.AddComponent<EntityEditorOnlyMetadata>();
#endif

		m_EntityHandles.push_back(EntityHandle { .Identifier = entity.m_EntityIdentifier });
		m_GUIDToEntityIdentifierMap.insert({guid, entity.m_EntityIdentifier});

		return entity;
	}

	Entity World::WrapIdentifierIntoEntity(EntityIdentifier identifier)
	{
		return Entity(*this, identifier);
	}

	void World::DestroyCommandEntity(DYE::DYEditor::Entity commandEntityToDestroy)
	{
		m_Registry.destroy(commandEntityToDestroy.m_EntityIdentifier);
	}

	void World::DestroyEntityAndChildren(Entity entityToDestroy)
	{
		auto tryGetIndex = TryGetEntityIndex(entityToDestroy);
		DYE_ASSERT(tryGetIndex.has_value());

		auto allChildren = EntityUtil::GetAllChildrenPreorder(entityToDestroy);

		// Where the entity is located at in the handle array.
		// We will use the same value for children later because the child index shifts down when we remove an entity at the index.
		auto const indexInHandleArray = tryGetIndex.value();

		// Destroy the entity.
		auto tryGetGUID = entityToDestroy.TryGetGUID();

		if (tryGetGUID.has_value())
		{
			auto guid = tryGetGUID.value();

			// Remove it from the GUID map if it has a GUID/IDComponent.
			m_GUIDToEntityIdentifierMap.erase(guid);

			// If the entity has a parent, we need to remove its GUID from the parent's children list.
			auto tryGetParent = entityToDestroy.TryGetComponent<ParentComponent>();
			if (tryGetParent.has_value())
			{
				auto &parent = tryGetParent.value().get();
				auto parentEntity = TryGetEntityWithGUID(parent.ParentGUID);
				auto &parentChildrenComponent = parentEntity->GetComponent<ChildrenComponent>();
				std::erase(parentChildrenComponent.ChildrenGUIDs, guid);
			}
		}

		m_EntityHandles.erase(m_EntityHandles.begin() + indexInHandleArray);
		m_Registry.destroy(entityToDestroy.m_EntityIdentifier);

		// Destroy all children entity.
		for (auto childEntity : allChildren)
		{
			auto tryGetChildGUID = childEntity.TryGetGUID();
			if (tryGetChildGUID.has_value())
			{
				// Remove it from the GUID map if it has a GUID/IDComponent.
				m_GUIDToEntityIdentifierMap.erase(tryGetChildGUID.value());
			}

			m_EntityHandles.erase(m_EntityHandles.begin() + indexInHandleArray);
			m_Registry.destroy(childEntity.m_EntityIdentifier);
		}
	}

	void World::DestroyEntityAndChildrenWithGUID(GUID entityGUID)
	{
		auto findItr = m_GUIDToEntityIdentifierMap.find(entityGUID);
		if (findItr == m_GUIDToEntityIdentifierMap.end())
		{
			// There is no entity with the given GUID recorded in the map, skip the function.
			return;
		}

		auto identifier = findItr->second;
		Entity entityToDestroy = WrapIdentifierIntoEntity(identifier);
		auto tryGetIndex = TryGetEntityIndex(entityToDestroy);
		DYE_ASSERT(tryGetIndex.has_value());

		auto allChildren = EntityUtil::GetAllChildrenPreorder(entityToDestroy);

		// Where the entity is located at in the handle array.
		// We will use the same value for children later because the child index shifts down when we remove an entity at the index.
		auto const indexInHandleArray = tryGetIndex.value();

		// If the entity has a parent, we need to remove its GUID from the parent's children list.
		auto tryGetParent = entityToDestroy.TryGetComponent<ParentComponent>();
		if (tryGetParent.has_value())
		{
			auto &parent = tryGetParent.value().get();
			auto parentEntity = TryGetEntityWithGUID(parent.ParentGUID);
			auto &parentChildrenComponent = parentEntity->GetComponent<ChildrenComponent>();
			std::erase(parentChildrenComponent.ChildrenGUIDs, entityGUID);
		}

		// Destroy the entity.
		m_GUIDToEntityIdentifierMap.erase(entityGUID);
		m_EntityHandles.erase(m_EntityHandles.begin() + indexInHandleArray);
		m_Registry.destroy(entityToDestroy.m_EntityIdentifier);

		// Destroy all children entity.
		for (auto childEntity : allChildren)
		{
			auto tryGetChildGUID = childEntity.TryGetGUID();
			if (tryGetChildGUID.has_value())
			{
				// Remove it from the GUID map if it has a GUID/IDComponent.
				m_GUIDToEntityIdentifierMap.erase(tryGetChildGUID.value());
			}

			m_EntityHandles.erase(m_EntityHandles.begin() + indexInHandleArray);
			m_Registry.destroy(childEntity.m_EntityIdentifier);
		}
	}

	Entity World::DuplicateEntityAndChildren(Entity rootEntityToDuplicate)
	{
		auto tryGetRootEntityGUID = rootEntityToDuplicate.TryGetGUID();
		DYE_ASSERT_LOG_WARN(tryGetRootEntityGUID.has_value(), "We can't duplicate an entity without GUID.");

		std::vector<GUID> newDuplicateGUIDs;
		std::vector<Entity> newEntityAndAllChildren;

		// Go through the entity and its children to make copies of them.
		struct HierarchyLevel
		{
			GUID NewParentGUID;
			ChildrenComponent *pChildrenComponent = nullptr;
			std::size_t NumberOfChildrenLeft = 0;
		};

		std::stack<Entity> entityStack;
		std::stack<HierarchyLevel> hierarchyLevels;
		entityStack.push(rootEntityToDuplicate);
		while (!entityStack.empty())
		{
			Entity entityToDuplicate = entityStack.top();

			auto tryGetName = entityToDuplicate.TryGetName();
			DYE_ASSERT_LOG_WARN(tryGetName.has_value(), "The duplicate entity target doesn't have a name.");

			// Make a new copy of the entity.
			Entity newEntity = this->createUntrackedEntity();

			SerializedEntity serializedEntityToDuplicate = SerializedObjectFactory::CreateSerializedEntity(entityToDuplicate);
			SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(serializedEntityToDuplicate, newEntity);

			// The newly duplicated entity should have a new GUID than the original one.
			GUID newGUID = m_EntityGUIDFactory.Generate();
			newEntity.GetComponent<IDComponent>().ID = newGUID;

			newDuplicateGUIDs.push_back(newGUID);
			newEntityAndAllChildren.push_back(newEntity);

			this->registerUntrackedEntityAtIndex(newEntity, m_EntityHandles.size());

			if (!hierarchyLevels.empty())
			{
				// The level stack is not empty, which means the current entity has a parent.
				// We need to reassign child's ParentComponent & parent's ChildrenComponent with the new GUIDs.
				HierarchyLevel &level = hierarchyLevels.top();

				newEntity.GetComponent<ParentComponent>().ParentGUID = level.NewParentGUID;

				std::size_t const numberOfChildren = level.pChildrenComponent->ChildrenGUIDs.size();
				level.pChildrenComponent->ChildrenGUIDs[numberOfChildren - level.NumberOfChildrenLeft] = newGUID;

				level.NumberOfChildrenLeft--;
				if (level.NumberOfChildrenLeft == 0)
				{
					hierarchyLevels.pop();
				}
			}

			// Check if the current visiting entity has children.
			// If so, we want to push it in the stack.
			entityStack.pop();
			auto tryGetChild = entityToDuplicate.TryGetComponent<ChildrenComponent>();
			if (!tryGetChild.has_value())
			{
				// No child, nothing to push into the stack.
				continue;
			}

			auto &childrenGUIDs = tryGetChild.value().get().ChildrenGUIDs;
			hierarchyLevels.push
			(
				HierarchyLevel
				{
					.NewParentGUID = newGUID,
					.pChildrenComponent = &newEntity.GetComponent<ChildrenComponent>(),
					.NumberOfChildrenLeft = childrenGUIDs.size()
				}
			);
			for (int i = childrenGUIDs.size() - 1; i >= 0; i--)
			{
				auto childGUID = childrenGUIDs[i];
				auto tryGetEntityWithGUID = this->TryGetEntityWithGUID(childGUID);
				if (!tryGetEntityWithGUID.has_value())
				{
					continue;
				}
				// Push the child into the stack.
				entityStack.push(tryGetEntityWithGUID.value());
			}
		}

		// FIXME: for now we remove the parent of the root.
		//  Later we want to be able to duplicate entity at the same level.
		Entity newRootEntity = newEntityAndAllChildren[0];
		if (newRootEntity.HasComponent<ParentComponent>())
		{
			newRootEntity.RemoveComponent<ParentComponent>();
		}

		return newRootEntity;
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
		return !m_Registry.storage<entt::entity>()->in_use();
	}

	void World::Reserve(std::size_t capacity)
	{
		m_EntityHandles.reserve(capacity);
		m_GUIDToEntityIdentifierMap.reserve(capacity);
		m_Registry.storage<entt::entity>().reserve(capacity);
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
		m_EntityHandles.insert(m_EntityHandles.begin() + index, EntityHandle { .Identifier = entity.m_EntityIdentifier });

		auto tryGetGUID = entity.TryGetGUID();
		DYE_ASSERT_LOG_WARN(tryGetGUID.has_value(), "The given entity %d doesn't have a GUID (i.e. IDComponent), cannot be tracked.", entity.GetIdentifier());

		auto guid = tryGetGUID.value();
		DYE_ASSERT_LOG_WARN(!m_GUIDToEntityIdentifierMap.contains(guid), "The given entity's GUID (%s) has already been tracked in the map.", guid.ToString().c_str());

		m_GUIDToEntityIdentifierMap.insert({tryGetGUID.value(), entity.m_EntityIdentifier});
	}

	void World::destroyEntityButNotChildren(Entity entity)
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

	void World::destroyEntityByGUIDButNotChildren(DYE::GUID entityGUID)
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
}