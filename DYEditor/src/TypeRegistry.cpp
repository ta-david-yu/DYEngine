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

	void TypeRegistry::RegisterSystem(const std::string &systemTypeName, SystemBase *systemInstance)
	{
		auto [iterator, insertionSuccess] = s_SystemRegistry.emplace(systemTypeName, systemInstance);
		if (!insertionSuccess)
		{
			DYE_LOG("A system with the name of '%s' has already been registered. Skip the registration with the same name.", systemTypeName.c_str());
			return;
		}

		DYE_LOG("Register system (name = %s)", systemTypeName.c_str());
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
		auto componentTypePairItr = s_ComponentTypeRegistry.find(componentTypeName);
		bool hasMatchedComponentType = componentTypePairItr != s_ComponentTypeRegistry.end();
		if (hasMatchedComponentType)
		{
			return componentTypePairItr->second;
		}

		auto formerlyKnownNamePairItr = s_FormerlyKnownTypeNames.find(componentTypeName);
		bool hasFormerlyKnownName = formerlyKnownNamePairItr != s_FormerlyKnownTypeNames.end();
		if (hasFormerlyKnownName)
		{
			std::string const &formerlyKnownName = formerlyKnownNamePairItr->second;
			componentTypePairItr = s_ComponentTypeRegistry.find(formerlyKnownName);
			hasMatchedComponentType = componentTypePairItr != s_ComponentTypeRegistry.end();
			if (hasMatchedComponentType)
			{
				return componentTypePairItr->second;
			}
		}

		return {};
	}

	SystemBase* TypeRegistry::TryGetSystemInstance(const std::string &systemName)
	{
		if (s_SystemRegistry.contains(systemName))
		{
			return s_SystemRegistry.at(systemName);
		}

		auto systemPairItr = s_SystemRegistry.find(systemName);
		bool hasMatchedSystem = systemPairItr != s_SystemRegistry.end();
		if (hasMatchedSystem)
		{
			return systemPairItr->second;
		}

		auto formerlyKnownNamePairItr = s_FormerlyKnownTypeNames.find(systemName);
		bool hasFormerlyKnownName = formerlyKnownNamePairItr != s_FormerlyKnownTypeNames.end();
		if (hasFormerlyKnownName)
		{
			std::string const &formerlyKnownName = formerlyKnownNamePairItr->second;
			systemPairItr = s_SystemRegistry.find(formerlyKnownName);
			hasMatchedSystem = systemPairItr != s_SystemRegistry.end();
			if (hasMatchedSystem)
			{
				return systemPairItr->second;
			}
		}

		return nullptr;
	}

	void TypeRegistry::RegisterFormerlyKnownTypeName(const std::string &formerlyKnownName, const std::string &currentTypeName)
	{
		auto [iterator, insertionSuccess] = s_FormerlyKnownTypeNames.emplace(formerlyKnownName, currentTypeName);
		if (!insertionSuccess)
		{
			DYE_LOG("A type that was formerly named as %s has already been registered in the formerly known type name map. "
					"Skip the registration with the same former name.", formerlyKnownName.c_str());
			return;
		}

		DYE_LOG("Register formerly known type name (%s -> %s)", formerlyKnownName.c_str(), currentTypeName.c_str());
	}

	void TypeRegistry::registerComponentType(std::string const &componentTypeName, ComponentTypeDescriptor componentDescriptor)
	{
		auto [iterator, insertionSuccess] = s_ComponentTypeRegistry.emplace(componentTypeName, componentDescriptor);
		if (!insertionSuccess)
		{
			DYE_LOG("A component type with the name of %s has already been registered. Skip the registration with the same name.", componentTypeName.c_str());
			return;
		}

		DYE_LOG("Register component type (typeName = %s)", componentTypeName.c_str());
	}
}