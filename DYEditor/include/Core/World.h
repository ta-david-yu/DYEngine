#pragma once

#include "WorldView.h"
#include "Core/GUID.h"

#include <entt/entt.hpp>

namespace DYE::DYEditor
{
	class Entity;

	class World
	{
		// We need this so Entity could access m_Registry for entt operation.
		friend Entity;
		friend entt::registry& GetWorldUnderlyingRegistry(World& world);
	public:
		World();
		~World() = default;

		Entity CreateEntity();
		Entity CreateEntity(std::string const& name);
		Entity CreateEntityWithGUID(std::string const& name, GUID guid);
		void DestroyEntity(Entity& entity);

		template<typename Func>
		void ForEachEntity(Func function)
		{
			m_Registry.each(
				[this, function](auto entityHandle)
				{
					Entity entity (*this, entityHandle);
					function(entity);
				});
		}

		inline bool IsEmpty() const { return m_Registry.empty(); }
		inline void Reserve(std::size_t size) { m_Registry.reserve(size); }
		void Clear() { m_Registry.clear<>(); }
	private:
		entt::registry m_Registry;
		GUIDFactory m_EntityGUIDFactory;

		// TODO: have a list of Entity (entity wrapper to keep track of UUID, entity metadata)

	};

	entt::registry& GetWorldUnderlyingRegistry(World& world);
}