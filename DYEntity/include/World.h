#pragma once

#include "WorldView.h"

#include <entt/entt.hpp>

namespace DYE::DYEntity
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

		// TODO: vector<Entity>?

	};

	entt::registry& GetWorldUnderlyingRegistry(World& world);
}