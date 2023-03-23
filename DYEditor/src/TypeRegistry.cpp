#include "TypeRegistry.h"

#include "EditorSystem.h"
#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> TypeRegistry::s_ComponentNamesAndFunctionCollectionsCache = {};
	std::vector<std::pair<std::string, SystemBase*>> TypeRegistry::s_SystemNamesAndPointersCache = {};

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
		auto registrySize = s_ComponentTypeRegistry.size();
		bool const cacheNeededRefresh = s_ComponentNamesAndFunctionCollectionsCache.size() != registrySize;
		if (!cacheNeededRefresh)
		{
			return s_ComponentNamesAndFunctionCollectionsCache;
		}

		s_ComponentNamesAndFunctionCollectionsCache.clear();
		s_ComponentNamesAndFunctionCollectionsCache.reserve(registrySize);
		for (auto const& pair : s_ComponentTypeRegistry)
		{
			s_ComponentNamesAndFunctionCollectionsCache.emplace_back(pair);
		}
		return s_ComponentNamesAndFunctionCollectionsCache;
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
		auto registrySize = s_SystemRegistry.size();
		bool const cacheNeededRefresh = s_SystemNamesAndPointersCache.size() != registrySize;
		if (!cacheNeededRefresh)
		{
			return s_SystemNamesAndPointersCache;
		}

		s_SystemNamesAndPointersCache.clear();
		s_SystemNamesAndPointersCache.reserve(registrySize);
		for (auto const& pair : s_SystemRegistry)
		{
			s_SystemNamesAndPointersCache.emplace_back(pair);
		}
		return s_SystemNamesAndPointersCache;
	}

	std::optional<ComponentTypeFunctionCollection> TypeRegistry::TryGetComponentTypeFunctions(std::string const& componentTypeName)
	{
		if (s_ComponentTypeRegistry.contains(componentTypeName))
		{
			return s_ComponentTypeRegistry.at(componentTypeName);
		}

		return {};
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