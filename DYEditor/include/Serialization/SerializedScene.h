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
		/// \return a SerializedSystemHandle of the newly added system OR the existing system of the given type.
		SerializedSystemHandle TryAddSystemOfType(std::string const& systemNode);

	private:
		SerializedScene() = default;
		explicit SerializedScene(toml::table &&sceneTable);
		toml::table m_SceneTable;
	};
}