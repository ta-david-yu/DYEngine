#pragma once

#include "SerializedComponent.h"

#include <toml++/toml.h>

namespace DYE::DYEditor
{
	struct SerializedEntity
	{
		friend class SerializedObjectFactory;
		friend class SerializedScene;

		SerializedEntity() = default;

		inline bool IsHandle() const { return m_IsHandle; }
		std::vector<SerializedComponent> GetSerializedComponentHandles();
		/// \return true if a component of the given type is removed.
		bool TryRemoveComponentHandleOfType(const std::string &typeName);
		/// \return a SerializedComponentHandle of the newly added component OR the existing component of the given type.
		SerializedComponent TryAddOrGetComponentHandleOfType(const std::string &typeName);

		/// Emplace the given serialized component as a serialized component handle.
		void PushSerializedComponent(const SerializedComponent &serializedComponent);

	private:
		explicit SerializedEntity(toml::table* pEntityTableHandle);
		explicit SerializedEntity(toml::table&& entityTable);

		// This will only be valid when IsHandle is false
		toml::table m_EntityTable;

		// This will only be valid when IsHandle is true. In that case it points to the entity table owned by the belonging SerializedScene.
		toml::table* m_pEntityTableHandle = nullptr;

		bool m_IsHandle = false;
	};
}