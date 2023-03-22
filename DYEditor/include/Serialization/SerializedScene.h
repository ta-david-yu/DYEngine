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
		std::optional <std::string> GetName() const;
		void SetName(std::string const &name);
		std::vector<SerializedSystemHandle> GetSerializedSystemHandles();
		std::vector<SerializedEntity> GetSerializedEntityHandles();

	private:
		SerializedScene() = default;
		explicit SerializedScene(toml::table &&sceneTable);
		toml::table m_SceneTable;
	};
}