#pragma once

#include "Core/EntityTypes.h"
#include "Core/WorldView.h"
#include "Core/GUID.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <concepts>

namespace DYE::DYEditor
{
	class Entity;
	class World
	{
		// We need this so Entity could access world.m_Registry for entt operation.
		friend class Entity;

		friend class Undo;
		friend class SerializedObjectFactory;

		friend class EntityCreationOperation;
		friend class EntityDeletionOperation;
		friend class EntityMoveOperation;

	public:
		World();
		~World() = default;

		/// Create a command entity that is supposed to live for a short life-time (i.e. one frame).
		/// It will not be shown inside inspector (for now), and will not be tracked by the serialization system.
		Entity CreateCommandEntity();

		Entity CreateEntity(std::string const& name);
		Entity CreateEntityAtIndex(std::string const& name, std::size_t index);
		Entity CreateEntityWithGUID(std::string const& name, GUID guid);
		Entity WrapIdentifierIntoEntity(EntityIdentifier identifier);

		/// This method only destroys the command entity itself in the registry.
		void DestroyCommandEntity(Entity commandEntityToDestroy);
		/// This method also destroys all the children under the entity.
		void DestroyEntityAndChildren(Entity entityToDestroy);
		/// This method also destroys all the children under the entity.
		void DestroyEntityAndChildrenWithGUID(GUID entityGUID);

		/// Duplicate an entity and all its children with new guids, and then put them at the end of the handle list.
		Entity DuplicateEntityAndChildren(Entity rootEntityToDuplicate);

		std::optional<Entity> TryGetEntityWithGUID(GUID entityGUID);
		/// Get the index of the given entity inside Entity Handle array.
		std::optional<std::size_t> TryGetEntityIndex(Entity const &entity);

		Entity GetEntityAtIndex(std::size_t index);

		template<typename Func>
		requires std::invocable<Func, DYE::DYEditor::Entity> || std::invocable<Func, DYE::DYEditor::Entity&>
		void ForEachEntity(Func function)
		{
			for (auto& entityHandle : m_EntityHandles)
			{
				Entity entity (*this, entityHandle.Identifier);
				function(entity);
			}
		}

		template<typename Func>
		requires std::invocable<Func, DYE::DYEditor::Entity, std::size_t> || std::invocable<Func, DYE::DYEditor::Entity&, std::size_t>
		void ForEachEntityAndIndex(Func function)
		{
			for (std::size_t i = 0; i < m_EntityHandles.size(); i++)
			{
				auto& entityHandle = m_EntityHandles[i];
				Entity entity (*this, entityHandle.Identifier);
				function(entity, i);
			}
		}

		bool IsEmpty() const;
		void Reserve(std::size_t capacity);
		void Clear();
		std::size_t GetNumberOfEntities() const { return m_EntityHandles.size(); }

		entt::registry &GetRegistry() { return m_Registry; };

	private:
		/// Create an empty entity that is not tracked by the internal Entity Handle array & GUID map.
		/// You need to make sure to call registerUntrackedEntityAtIndex after adding an IDComponent to the entity.
		/// This method is for engine-internally use (e.g. Deserialization, Undo/redo entity creation etc).
		Entity createUntrackedEntity();

		/// This method is for engine-internally use (e.g. Deserialization, Undo/redo entity creation etc).
		void registerUntrackedEntityAtIndex(Entity entity, std::size_t index);

		/// This method doesn't destroy the entity's children & update related hierarchy components.
		/// It is meant for engine-internally use (e.g. Undo/redo entity destruction).
		/// You should avoid using this on a single entity but instead on a batch of entity hierarchy.
		void destroyEntityButNotChildren(Entity entity);

		/// This method doesn't destroy the entity's children & update related hierarchy components.
		/// It is meant for engine-internally use (e.g. Undo/redo entity destruction).
		/// You should avoid using this on a single entity but instead on a batch of entity hierarchy.
		void destroyEntityByGUIDButNotChildren(DYE::GUID entityGUID);

		/// Refresh all the entity cache stored in hierarchy components such as Parent and Children components.
		void refreshAllHierarchyComponentEntityCache();

	private:
		struct EntityHandle
		{
			EntityIdentifier Identifier;
		};

		GUIDFactory m_EntityGUIDFactory;

		std::vector<EntityHandle> m_EntityHandles;

		using Map = std::unordered_map<GUID, EntityIdentifier>;
		Map m_GUIDToEntityIdentifierMap;

		entt::registry m_Registry;
	};
}