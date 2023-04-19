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

		SerializedEntity CreateAndAddSerializedEntity();

		struct AddSystemParameters
		{
			std::string SystemTypeName;
			bool HasGroup = false;
			std::string SystemGroupName;
			bool IsEnabled = true;
		};
		/// \return a SerializedSystemHandle of the newly added system OR the existing system of the given type.
		SerializedSystemHandle TryAddSystem(AddSystemParameters addSystemParams);

	private:
		SerializedScene() = default;
		explicit SerializedScene(toml::table &&sceneTable);
		toml::table m_SceneTable;
	};
}