#include "Core/Entity.h"

#include "Core/World.h"
#include "Components/NameComponent.h"

namespace DYE::DYEditor
{
	Entity::Entity(World &world, EntityIdentifier identifier) : m_World(&world), m_EntityIdentifier(identifier)
	{
	}

	bool Entity::IsValid() const
	{
		if (m_World == nullptr)
		{
			return false;
		}

		bool const isNullEntity = m_EntityIdentifier == entt::null;
		return !isNullEntity && m_World->m_Registry.valid(m_EntityIdentifier);
	}

	std::optional<std::string> Entity::TryGetName()
	{
		if (!this->HasComponent<NameComponent>())
		{
			return {};
		}

		return this->GetComponent<NameComponent>().Name;
	}

	void Entity::RemoveAllComponents()
	{
		for (auto [id, storage] : m_World->m_Registry.storage())
		{
			storage.remove(m_EntityIdentifier);
		}
	}

}