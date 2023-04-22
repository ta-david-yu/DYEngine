#pragma once

#include "Core/WorldView.h"
#include "Core/GUID.h"

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

		Entity CreateEntity();
		Entity CreateEntity(std::string const& name);
		Entity CreateEntityWithGUID(std::string const& name, GUID guid);
		void DestroyEntity(Entity& entity);
		void DestroyEntity(EntityIdentifier identifier);

		template<typename Func>
		void ForEachEntity(Func function)
		{
			for (auto& entityHandle : m_EntityHandles)
			{
				Entity entity (*this, entityHandle.Identifier);
				function(entity);
			}
		}

		bool IsEmpty() const;
		void Reserve(std::size_t size);
		void Clear();

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