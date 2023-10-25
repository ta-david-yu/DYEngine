#pragma once

#include "Core/GUID.h"
#include "Core/EntityTypes.h"

namespace DYE::DYEditor
{
	class Entity;
	class World;

	struct ParentComponent
	{
		GUID GetParentGUID() const { return m_ParentGUID; }
		EntityIdentifier GetParentIdentifier() const { return m_ParentEntityIdentifierCache; }
		Entity GetParent(World &world);

		/// This function set the guid and reset all the entity cache.
		/// \param guid
		void SetParentGUID(GUID guid);
		void SetParent(Entity &entity);
		void SetParent(EntityIdentifier identifier, GUID guid);

		/// Try to set parent guid and cached entity identifier if an entity with the guid is found in the given world.
		/// \param guid
		/// \param world
		/// \return whether or not an entity with the given guid can be found in the given world.
		bool TrySetParentGUIDIfFoundInWorld(GUID guid, World &world);

		void RefreshEntityIdentifierCache(World &world);

	private:
		GUID m_ParentGUID;
		EntityIdentifier m_ParentEntityIdentifierCache = entt::null;
	};

	struct ChildrenComponent
	{
		std::vector<GUID> ChildrenGUIDs;
	};
}