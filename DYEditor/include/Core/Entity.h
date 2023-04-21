#pragma once

#include "Util/Macro.h"
#include "Core/World.h"

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <entt/entt.hpp>

namespace DYE::DYEditor
{
	/// We want to have a light-weight wrapper around the underlying implementation.
	/// The wrapper Entity class should be trivially-copyable.
	class Entity
	{
		// We need this so World could call the private ctor.
		friend class World;
	public:
		/// Create a null entity.
		Entity() = default;
		Entity(Entity const& other) = default;

		EntityInstanceID GetInstanceID() const { return static_cast<EntityInstanceID>(m_EntityIdentifier); }
		bool IsValid() const;

		template<typename T, typename... Args>
		T& AddComponent(Args&&...args)
		{
			DYE_ASSERT(!this->HasComponent<T>() && "Entity::AddComponent: Entity already has component of the given type.");
			return m_World->m_Registry.emplace<T>(m_EntityIdentifier, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& AddOrGetComponent(Args&&...args)
		{
			if (this->HasComponent<T>())
			{
				return GetComponent<T>();
			}
			return m_World->m_Registry.emplace<T>(m_EntityIdentifier, std::forward<Args>(args)...);
		}

		/// \return the number of removed components. 0 if the entity doesn't own the given component.
		template<typename T>
		std::size_t RemoveComponent()
		{
			return m_World->m_Registry.remove<T>(m_EntityIdentifier);
		}

		template<typename T>
		T& GetComponent()
		{
			DYE_ASSERT(this->HasComponent<T>() && "Entity::GetComponent: Entity doesn't have component of the given type.");
			return m_World->m_Registry.get<T>(m_EntityIdentifier);
		}

		/// Syntactic sugar for TryGetComponent<NameComponent>().Name
		/// \return
		std::optional<std::string> TryGetName();

		template<typename T>
		std::optional<std::reference_wrapper<T>> TryGetComponent()
		{
			if (!this->HasComponent<T>())
			{
				return {};
			}
			return m_World->m_Registry.get<T>(m_EntityIdentifier);
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_World->m_Registry.all_of<T>(m_EntityIdentifier);
		}

		void RemoveAllComponents();

		bool operator==(Entity const& other) const
		{
			return m_EntityIdentifier == other.m_EntityIdentifier && m_World == other.m_World;
		}

		bool operator!=(Entity const& other) const
		{
			return !(*this == other);
		}

	private:
		/// Create an Entity with the given World & the internal handle.
		explicit Entity(World& world, EntityIdentifier identifier);

		World* m_World = nullptr;
		EntityIdentifier m_EntityIdentifier = entt::null;
	};
}