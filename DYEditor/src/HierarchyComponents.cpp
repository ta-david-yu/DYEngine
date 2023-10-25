#include "Components/HierarchyComponents.h"

#include "Util/Macro.h"
#include "Core/Entity.h"
#include "Core/World.h"

namespace DYE::DYEditor
{
	Entity ParentComponent::GetParent(World &world)
	{
		if (m_ParentEntityIdentifierCache != entt::null)
		{
			return world.WrapIdentifierIntoEntity(m_ParentEntityIdentifierCache);
		}

		auto tryGetEntity = world.TryGetEntityWithGUID(m_ParentGUID);
		if (!tryGetEntity.has_value())
		{
			DYE_LOG("ParentComponent::Get: Cannot find the parent entity with GUID '%s'.", m_ParentGUID.ToString().c_str());
			return {};
		}

		m_ParentEntityIdentifierCache = tryGetEntity->GetIdentifier();
		return tryGetEntity.value();
	}

	void ParentComponent::SetParentGUID(GUID guid)
	{
		m_ParentGUID = guid;
		m_ParentEntityIdentifierCache = entt::null;
	}

	void ParentComponent::SetParent(Entity &entity)
	{
		auto tryGetGUID = entity.TryGetGUID();
		DYE_ASSERT_LOG_WARN(tryGetGUID.has_value(), "Try to set parent, but the given parent entity '%d' doesn't have a GUID.", entity.GetIdentifier());

		m_ParentGUID = tryGetGUID.value();
		m_ParentEntityIdentifierCache = entity.GetIdentifier();
	}

	void ParentComponent::SetParent(EntityIdentifier identifier, GUID guid)
	{
		m_ParentGUID = guid;
		m_ParentEntityIdentifierCache = identifier;
	}

	bool ParentComponent::TrySetParentGUIDIfFoundInWorld(GUID guid, World &world)
	{
		auto tryGetEntity = world.TryGetEntityWithGUID(guid);
		if (!tryGetEntity.has_value())
		{
			DYE_LOG("ParentComponent::TrySetParentGUIDIfFoundInWorld: Cannot find the entity with GUID '%s' in the world.", guid.ToString().c_str());
			return false;
		}

		m_ParentGUID = guid;
		m_ParentEntityIdentifierCache = tryGetEntity.value().GetIdentifier();
		return true;
	}

	void ParentComponent::RefreshEntityIdentifierCache(World &world)
	{
		auto tryGetEntity = world.TryGetEntityWithGUID(m_ParentGUID);
		if (!tryGetEntity.has_value())
		{
			DYE_LOG("ParentComponent::RefreshEntityIdentifierCache: Cannot find the entity with GUID '%s' in the world.", m_ParentGUID.ToString().c_str());
			m_ParentEntityIdentifierCache = entt::null;
			return;
		}

		m_ParentEntityIdentifierCache = tryGetEntity.value().GetIdentifier();
	}

	std::optional<GUID> ChildrenComponent::TryGetChildGUIDAt(int index) const
	{
		if (index < m_ChildrenGUIDs.size())
		{
			return m_ChildrenGUIDs[index];
		}

		return {};
	}

	void ChildrenComponent::SetChildGUIDAt(std::size_t index, GUID guid)
	{
		DYE_ASSERT_LOG_WARN(index < m_ChildrenGUIDs.size(), "Try to set the child guid at index '%zu', but the index is out of bounds.", index);
		m_ChildrenGUIDs[index] = guid;

		m_ChildrenEntityIdentifiersCache[index] = entt::null;
	}

	void ChildrenComponent::InsertChildGUIDAt(std::size_t index, GUID guid)
	{
		m_ChildrenGUIDs.insert(m_ChildrenGUIDs.begin() + index, guid);

		m_ChildrenEntityIdentifiersCache.insert(m_ChildrenEntityIdentifiersCache.begin() + index, (EntityIdentifier) entt::null);
	}

	void ChildrenComponent::PushBackWithGUID(GUID guid)
	{
		m_ChildrenGUIDs.push_back(guid);

		m_ChildrenEntityIdentifiersCache.push_back(entt::null);
	}

	void ChildrenComponent::RemoveChildWithGUID(GUID guid)
	{
		for (int i = m_ChildrenGUIDs.size() - 1; i >= 0; i--)
		{
			if (m_ChildrenGUIDs[i] == guid)
			{
				m_ChildrenGUIDs.erase(m_ChildrenGUIDs.begin() + i);
				m_ChildrenEntityIdentifiersCache.erase(m_ChildrenEntityIdentifiersCache.begin() + i);
			}
		}
	}

	void ChildrenComponent::PopBack()
	{
		m_ChildrenGUIDs.pop_back();
		m_ChildrenEntityIdentifiersCache.pop_back();
	}

	void ChildrenComponent::RefreshChildrenEntityIdentifierCache(World &world)
	{
		m_ChildrenEntityIdentifiersCache.resize(m_ChildrenGUIDs.size());

		for (auto i = 0; i < m_ChildrenGUIDs.size(); i++)
		{
			auto guid = m_ChildrenGUIDs[i];

			auto tryGetEntity = world.TryGetEntityWithGUID(guid);
			if (!tryGetEntity.has_value())
			{
				DYE_LOG("ChildrenComponent::RefreshChildrenEntityIdentifierCache: Child (GUID=%s) at index %d doesn't have a matching entity in the world.", guid.ToString().c_str(), i);
				m_ChildrenEntityIdentifiersCache[i] = entt::null;
				continue;
			}

			m_ChildrenEntityIdentifiersCache[i] = tryGetEntity.value().GetIdentifier();
		}
	}

}