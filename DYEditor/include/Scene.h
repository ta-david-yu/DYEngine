#pragma once

#include "World.h"

#include <vector>
#include <string>

namespace DYE::DYEditor
{
	class Scene
	{
		friend class SerializedObjectFatory;

	public:
		std::string Name;
		// At runtime, we will use the names to reference the actual systems from TypeRegistry.
		std::vector<std::string> SystemTypeNames;
		DYEntity::World World;

	private:
		Scene() = default;
	};
}