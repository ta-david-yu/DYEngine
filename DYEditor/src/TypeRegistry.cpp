#include "Type/TypeRegistry.h"

#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	std::vector<std::pair<std::string, ComponentTypeDescriptor>> TypeRegistry::s_ComponentNamesAndDescriptorsCache = {};
	std::vector<std::pair<std::string, SystemBase*>> TypeRegistry::s_SystemNamesAndPointersCache = {};

	void TypeRegistry::ClearRegisteredComponentTypes()
	{
		s_ComponentTypeRegistry.clear();
	}

	void TypeRegistry::ClearRegisteredSystems()
	{
		s_SystemRegistry.clear();
	}

	std::vector<std::pair<std::string, ComponentTypeDescriptor>> TypeRegistry::GetComponentTypesNamesAndDescriptors()
	{
		auto registrySize = s_ComponentTypeRegistry.size();
		bool const cacheNeededRefresh = s_ComponentNamesAndDescriptorsCache.size() != registrySize;
		if (!cacheNeededRefresh)
		{
			return s_ComponentNamesAndDescriptorsCache;
		}

		s_ComponentNamesAndDescriptorsCache.clear();
		s_ComponentNamesAndDescriptorsCache.reserve(registrySize);
		for (auto const& pair : s_ComponentTypeRegistry)
		{
			s_ComponentNamesAndDescriptorsCache.emplace_back(pair);
		}
		return s_ComponentNamesAndDescriptorsCache;
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

	std::optional<ComponentTypeDescriptor> TypeRegistry::TryGetComponentTypeDescriptor(std::string const& componentTypeName)
	{
		if (s_ComponentTypeRegistry.contains(componentTypeName))
		{
			return s_ComponentTypeRegistry.at(componentTypeName);
		}

		return {};
	}

	SystemBase* TypeRegistry::TryGetSystemInstance(const std::string &systemName)
	{
		if (s_SystemRegistry.contains(systemName))
		{
			return s_SystemRegistry.at(systemName);
		}

		return nullptr;
	}

	void TypeRegistry::registerComponentType(std::string const &componentName, ComponentTypeDescriptor componentDescriptor)
	{
		auto [iterator, insertionSuccess] = s_ComponentTypeRegistry.emplace(componentName, componentDescriptor);
		if (!insertionSuccess)
		{
			DYE_LOG("A component type with the name of %s has already been registered. Skip the registration with the same name.", componentName.c_str());
			return;
		}

		DYE_LOG("Register component type (name = %s)", componentName.c_str());
	}
}