#include "../include/Core/Entity.h"

#include "../include/Components/NameComponent.h"

namespace DYE::DYEditor
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

		bool const isNullEntity = m_EntityHandle == entt::null;
		return !isNullEntity && m_World->m_Registry.valid(m_EntityHandle);
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
			storage.remove(m_EntityHandle);
		}
	}

}