#pragma once

#include "Core/WorldView.h"
#include "Core/GUID.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>

namespace DYE::DYEditor
{
	using EntityIdentifier = entt::entity;
	using EntityInstanceID = std::uint32_t;

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

		friend entt::registry& GetWorldUnderlyingRegistry(World& world);
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

		std::optional<Entity> TryGetEntityWithGUID(GUID entityGUID);
		/// Get the index of the given entity inside Entity Handle array.
		std::optional<std::size_t> TryGetEntityIndex(Entity const &entity);

		Entity GetEntityAtIndex(std::size_t index);

		template<typename Func>
		void ForEachEntity(Func function)
		{
			for (auto& entityHandle : m_EntityHandles)
			{
				Entity entity (*this, entityHandle.Identifier);
				function(entity);
			}
		}

		template<typename Func>
		void ForEachEntityAndIndex(Func function)
		{
			for (std::size_t i = 0; i < m_EntityHandles.size(); i++)
			{
				auto& entityHandle = m_EntityHandles[i];
				Entity entity (*this, entityHandle.Identifier);
				function(entity, i);
			}
		}

		template<typename Type, typename... Types, typename... ExcludeTypes>
		auto GetView(Exclude_t<ExcludeTypes...> excludes = {})
		{
			return m_Registry.view<Type, Types..., ExcludeTypes...>(excludes);
		}

		template<typename Type, typename... Types, typename... ExcludeTypes>
		auto GetView(Exclude_t<ExcludeTypes...> excludes = {}) const
		{
			return m_Registry.view<Type, Types..., ExcludeTypes...>(excludes);
		}

		template<typename... OwnedTypes, typename... GetTypes, typename... ExcludeTypes>
		auto GetGroup(Get_t<GetTypes...> gets = {}, Exclude_t<ExcludeTypes...> excludes = {})
		{
			return m_Registry.group<OwnedTypes...>(gets, excludes);
		}

		template<typename... OwnedTypes, typename... GetTypes, typename... ExcludeTypes>
		auto GetGroup(Get_t<GetTypes...> gets = {}, Exclude_t<ExcludeTypes...> excludes = {}) const
		{
			return m_Registry.group<OwnedTypes...>(gets, excludes);
		}

		bool IsEmpty() const;
		void Reserve(std::size_t size);
		void Clear();
		std::size_t GetNumberOfEntities() const { return m_EntityHandles.size(); }

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

	entt::registry& GetWorldUnderlyingRegistry(World& world);
}