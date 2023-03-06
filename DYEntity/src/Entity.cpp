#include "Entity.h"

namespace DYE::DYEntity
{
	Entity::Entity(World &world) : m_World(&world), m_EntityHandle(world.m_Registry.create())
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
}