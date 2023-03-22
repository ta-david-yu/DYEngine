#include "Entity.h"

namespace DYE::DYEntity
{
	Entity::Entity(World &world, EntityHandle handle) : m_World(&world), m_EntityHandle(handle)
	{
	}

	bool Entity::IsValid() const
	{
		if (m_World == nullptr)
		{
			return false;
		}

		return m_EntityHandle != entt::null;;
	}

	void Entity::RemoveAllComponents()
	{
		for (auto [id, storage] : m_World->m_Registry.storage())
		{
			storage.remove(m_EntityHandle);
		}
	}
}