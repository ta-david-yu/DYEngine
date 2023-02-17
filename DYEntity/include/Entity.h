#pragma once

#include "Util/Macro.h"
#include "World.h"

#include <entt/entt.hpp>

namespace DYE::DYEntity
{
	using EntityHandle = entt::entity;

	/// We want to have a light-weight wrapper around the underlying implementation.
	/// The wrapper Entity class should be trivially-copyable.
	class Entity
	{
	public:
		/// Create a null entity.
		Entity() = default;

		/// Create an entity with the given World.
		explicit Entity(World& world);
		Entity(Entity const& other) = default;

		bool IsValid() const;

		template<typename T, typename... Args>
		T& AddComponent(Args&&...args)
		{
			DYE_ASSERT(!this->HasComponent<T>() && "Entity::AddComponent: Entity already has component of the given type.");
			return m_World->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			DYE_ASSERT(this->HasComponent<T>() && "Entity::GetComponent: Entity doesn't have component of the given type.");
			return m_World->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_World->m_Registry.all_of<T>(m_EntityHandle);
		}

	private:
		World* m_World = nullptr;
		EntityHandle m_EntityHandle = entt::null;
	};
}