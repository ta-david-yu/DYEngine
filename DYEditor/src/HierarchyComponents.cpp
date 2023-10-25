#include "Components/HierarchyComponents.h"

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
}