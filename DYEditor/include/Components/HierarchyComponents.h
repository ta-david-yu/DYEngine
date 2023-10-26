#pragma once

#include "Core/GUID.h"
#include "Core/EntityTypes.h"

#include <optional>

namespace DYE::DYEditor
{
	class Entity;
	class World;

	class SerializedComponent;

	struct SerializationResult;
	struct DeserializationResult;
	struct DrawComponentInspectorContext;
	namespace BuiltInComponentTypeFunctions
	{
		/// The actual implementation is located at BuiltInTypeRegister.cpp
		DeserializationResult ChildrenComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity);
		/// The actual implementation is located at BuiltInTypeRegister.cpp
		bool ChildrenComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity);
	}

	struct ParentComponent
	{
		GUID GetParentGUID() const { return m_ParentGUID; }
		Entity GetParent(World &world);

		/// This function set the guid and reset the entity identifier cache.
		/// Normally you should avoid using this function to set parent, use Entity.SetParent instead.
		void SetParentGUID(GUID guid);
		/// Normally you should avoid using this function to set parent, use Entity.SetParent instead.
		void SetParent(Entity &entity);
		/// Normally you should avoid using this function to set parent, use Entity.SetParent instead.
		void SetParent(EntityIdentifier identifier, GUID guid);

		/// Try to set parent guid and cached entity identifier if an entity with the guid is found in the given world.
		/// Normally you should avoid using this function to set parent, use Entity.SetParent instead.
		/// \return whether or not an entity with the given guid can be found in the given world.
		bool TrySetParentGUIDIfFoundInWorld(GUID guid, World &world);

		void RefreshEntityIdentifierCache(World &world);

	private:
		GUID m_ParentGUID;
		EntityIdentifier m_ParentEntityIdentifierCache = entt::null;
	};

	struct ChildrenComponent
	{
		std::size_t GetChildrenCount() const { return m_ChildrenGUIDs.size(); }
		std::vector<GUID> const& GetChildrenGUIDs() const { return m_ChildrenGUIDs; }
		std::optional<GUID> TryGetChildGUIDAt(int index) const;
		std::optional<Entity> TryGetChildAt(int index, World &world);

		void SetChildGUIDAt(std::size_t index, GUID guid);
		void InsertChildGUIDAt(std::size_t index, GUID guid);
		void PushBackWithGUID(GUID guid);

		/// \return the number of removed children.
		std::size_t RemoveChildWithGUID(GUID guid);
		void PopBack();

		void RefreshChildrenEntityIdentifierCache(World &world);

	private:
		std::vector<GUID> m_ChildrenGUIDs;
		std::vector<EntityIdentifier> m_ChildrenEntityIdentifiersCache;

	private:
		friend DeserializationResult BuiltInComponentTypeFunctions::ChildrenComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity);
		friend bool BuiltInComponentTypeFunctions::ChildrenComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity);
	};
}