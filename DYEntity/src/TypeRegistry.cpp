#include "TypeRegistry.h"

#include "Util/Logger.h"

namespace DYE::DYEntity
{
	void TypeRegistry::registerComponentType(std::string const &componentName, ComponentTypeFunctionCollection functions)
	{
		auto [iterator, insertionSuccess] = s_ComponentTypeRegistry.emplace(componentName, functions);
		if (!insertionSuccess)
		{
			DYE_LOG("A component type with the name of %s has already been registered. Skip the registration with the same name.", componentName.c_str());
			return;
		}

		DYE_LOG("Register component type (name = %s)", componentName.c_str());
	}

	std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> TypeRegistry::GetComponentTypesNamesAndFunctionCollections()
	{
		std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> types;
		for (auto const& pair : s_ComponentTypeRegistry)
		{
			types.emplace_back(pair);
		}
		return types;
	}

	std::vector<std::string> TypeRegistry::GetComponentTypeNames()
	{
		std::vector<std::string> names;
		for (auto const& pair : s_ComponentTypeRegistry)
		{
			names.push_back(pair.first);
		}
		return names;
	}

	std::vector<ComponentTypeFunctionCollection> TypeRegistry::GetComponentTypeFunctionCollections()
	{
		std::vector<ComponentTypeFunctionCollection> functions;
		for (auto const& pair : s_ComponentTypeRegistry)
		{
			functions.push_back(pair.second);
		}
		return functions;
	}

	std::optional<ComponentTypeFunctionCollection> TypeRegistry::TryGetComponentTypeFunctionsFromName(const std::string &componentName)
	{
		if (s_ComponentTypeRegistry.contains(componentName))
		{
			return s_ComponentTypeRegistry.at(componentName);
		}

		return {};
	}
}