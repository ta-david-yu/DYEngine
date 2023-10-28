#include "Core/Entity.h"

#include "Core/World.h"
#include "Components/IDComponent.h"
#include "Components/NameComponent.h"

namespace DYE::DYEditor
{
	Entity Entity::Null()
	{
		static Entity nullEntity;
		return nullEntity;
	}

	Entity::Entity(World &world, EntityIdentifier identifier) : m_pWorld(&world), m_EntityIdentifier(identifier)
	{
	}

	bool Entity::IsValid() const
	{
		if (m_pWorld == nullptr)
		{
			return false;
		}

		bool const isNullEntity = m_EntityIdentifier == entt::null;
		return !isNullEntity && m_pWorld->m_Registry.valid(m_EntityIdentifier);
	}

	std::optional<std::string> Entity::TryGetName()
	{
		if (!this->HasComponent<NameComponent>())
		{
			return {};
		}

		return this->GetComponent<NameComponent>().Name;
	}

	std::string Entity::GetName()
	{
		return this->GetComponent<NameComponent>().Name;
	}

	std::optional<GUID> Entity::TryGetGUID()
	{
		if (!this->HasComponent<IDComponent>())
		{
			return {};
		}

		return this->GetComponent<IDComponent>().ID;
	}

	void Entity::RemoveAllComponents()
	{
		for (auto [id, storage] : m_pWorld->m_Registry.storage())
		{
			storage.remove(m_EntityIdentifier);
		}
	}
}