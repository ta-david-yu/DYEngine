#pragma once

#include "Core/WorldView.h"
#include "Core/GUID.h"

#include <optional>
#include <vector>
#include <entt/entt.hpp>

namespace DYE::DYEditor
{
	using EntityIdentifier = entt::entity;
	using EntityInstanceID = std::uint32_t;

	class Entity;

	class World
	{
		// We need this so Entity could access m_Registry for entt operation.
		friend class Entity;
		friend entt::registry& GetWorldUnderlyingRegistry(World& world);
	public:
		World();
		~World() = default;

		/// Create an entity at the given location inside the internal Entity Handle array.
		/// This method is for engine-internally use (e.g. Undo/redo entity creation etc).
		Entity CreateEntityAtIndex(std::size_t index);
		Entity CreateEntity();
		Entity CreateEntity(std::string const& name);
		Entity CreateEntityWithGUID(std::string const& name, GUID guid);
		void DestroyEntity(Entity& entity);
		void DestroyEntity(EntityIdentifier identifier);
		/// Destroy an entity with the given GUID.
		/// The operation is very slow because we have to iterate through every entities' ID components.
		void DestroyEntityWithGUID(GUID entityGUID);
		std::optional<Entity> TryGetEntityWithGUID(GUID entityGUID);
		/// Get the index of the given entity inside Entity Handle array.
		std::optional<std::size_t> TryGetEntityIndex(Entity &entity);

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
		void ForEachEntityWithIndex(Func function)
		{
			for (std::size_t i = 0; i < m_EntityHandles.size(); i++)
			{
				auto& entityHandle = m_EntityHandles[i];
				Entity entity (*this, entityHandle.Identifier);
				function(entity, i);
			}
		}

		bool IsEmpty() const;
		void Reserve(std::size_t size);
		void Clear();
		std::size_t GetNumberOfEntities() const { return m_EntityHandles.size(); }


	private:
		struct EntityHandle
		{
			EntityIdentifier Identifier;
		};

		GUIDFactory m_EntityGUIDFactory;
		entt::registry m_Registry;
		std::vector<EntityHandle> m_EntityHandles;
	};

	entt::registry& GetWorldUnderlyingRegistry(World& world);
}