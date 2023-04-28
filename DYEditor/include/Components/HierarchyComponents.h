#pragma once

#include "Core/GUID.h"

namespace DYE::DYEditor
{
	class Entity;
	class World;

	struct ParentComponent
	{
		GUID ParentGUID;

		/// Right now this is super inefficient.
		Entity Get(World &world) const;
	};

	struct ChildrenComponent
	{
		std::vector<GUID> ChildrenGUIDs;
	};
}