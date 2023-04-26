#pragma once

#include "SerializedEntity.h"
#include "SerializedSystemHandle.h"

#include <vector>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	struct SerializedScene
	{
		friend class SerializedObjectFactory;

		std::optional <std::string> TryGetName() const;
		void SetName(std::string const &name);

		std::vector<SerializedSystemHandle> GetSerializedSystemHandles();
		std::vector<SerializedEntity> GetSerializedEntityHandles();

		/// \return a SerializedEntity of the newly created entity.
		SerializedEntity CreateAndAddEntityHandle();

		struct AddSystemParameters
		{
			std::string SystemTypeName;
			bool HasGroup = false;
			std::string SystemGroupName;
			bool IsEnabled = true;
		};

		/// Add a serialized system handle with the given parameters.
		/// If a system handle of specified type already exists, it will be overwritten with the given parameters.
		/// \return a SerializedSystemHandle of the newly added/replaced system.
		SerializedSystemHandle AddOrReplaceSystemHandle(AddSystemParameters addSystemParams);

	private:
		SerializedScene() = default;
		explicit SerializedScene(toml::table &&sceneTable);
		toml::table m_SceneTable;
	};
}