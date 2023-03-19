#include "TypeRegistry.h"

#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	void TypeRegistry::ClearRegisteredComponentTypes()
	{
		s_ComponentTypeRegistry.clear();
	}

	void TypeRegistry::ClearRegisteredSystems()
	{
		s_SystemRegistry.clear();
	}


	std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> TypeRegistry::GetComponentTypesNamesAndFunctionCollections()
	{
		std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> types;
		types.reserve(s_ComponentTypeRegistry.size());
		for (auto const& pair : s_ComponentTypeRegistry)
		{
			types.emplace_back(pair);
		}
		return types;
	}

	void TypeRegistry::RegisterSystem(const std::string &systemName, SystemBase *systemInstance)
	{
		auto [iterator, insertionSuccess] = s_SystemRegistry.emplace(systemName, systemInstance);
		if (!insertionSuccess)
		{
			DYE_LOG("A system with the name of %s has already been registered. Skip the registration with the same name.", systemName.c_str());
			return;
		}

		DYE_LOG("Register system (name = %s)", systemName.c_str());
	}

	std::vector<std::pair<std::string, SystemBase*>> TypeRegistry::GetSystemNamesAndInstances()
	{
		std::vector<std::pair<std::string, SystemBase*>> systems;
		systems.reserve(s_SystemRegistry.size());
		for (auto const& pair : s_SystemRegistry)
		{
			systems.emplace_back(pair);
		}
		return systems;
	}

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
}