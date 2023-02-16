#pragma once

#include <string>

namespace DYE::DYEntity
{
	struct NameComponent
	{
		std::string Name;
		explicit NameComponent(std::string const& name) : Name(name) {}
	};
}
