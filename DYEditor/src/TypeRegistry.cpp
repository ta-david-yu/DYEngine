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

	TypeRegistry::TryGetComponentTypeDescriptorResult TypeRegistry::TryGetComponentTypeDescriptor(std::string const& componentTypeName)
	{
		TryGetComponentTypeDescriptorResult result;

		auto componentTypePairItr = s_ComponentTypeRegistry.find(componentTypeName);
		bool hasMatchedComponentType = componentTypePairItr != s_ComponentTypeRegistry.end();
		if (hasMatchedComponentType)
		{
			result.Success = true;
			result.Descriptor = componentTypePairItr->second;
			result.FullTypeName = componentTypeName.c_str();
			return result;
		}

		auto formerlyKnownNamePairItr = s_FormerlyKnownTypeNames.find(componentTypeName);
		bool isInputFormerlyKnownName = formerlyKnownNamePairItr != s_FormerlyKnownTypeNames.end();
		if (isInputFormerlyKnownName)
		{
			std::string const &currentFullTypeName = formerlyKnownNamePairItr->second;
			componentTypePairItr = s_ComponentTypeRegistry.find(currentFullTypeName);
			hasMatchedComponentType = componentTypePairItr != s_ComponentTypeRegistry.end();
			if (hasMatchedComponentType)
			{
				result.Success = true;
				result.Descriptor = componentTypePairItr->second;
				result.FullTypeName = currentFullTypeName.c_str();
				return result;
			}
		}

		result.Success = false;
		return result;
	}

	TypeRegistry::TryGetSystemInstanceResult TypeRegistry::TryGetSystemInstance(const std::string &systemName)
	{
		TryGetSystemInstanceResult result;

		auto systemPairItr = s_SystemRegistry.find(systemName);
		bool hasMatchedSystem = systemPairItr != s_SystemRegistry.end();
		if (hasMatchedSystem)
		{
			result.Success = true;
			result.pInstance = systemPairItr->second;
			result.FullTypeName = systemName.c_str();
			return result;
		}

		auto formerlyKnownNamePairItr = s_FormerlyKnownTypeNames.find(systemName);
		bool isInputFormerlyKnownName = formerlyKnownNamePairItr != s_FormerlyKnownTypeNames.end();
		if (isInputFormerlyKnownName)
		{
			std::string const &currentFullTypeName = formerlyKnownNamePairItr->second;
			systemPairItr = s_SystemRegistry.find(currentFullTypeName);
			hasMatchedSystem = systemPairItr != s_SystemRegistry.end();
			if (hasMatchedSystem)
			{
				result.Success = true;
				result.pInstance = systemPairItr->second;
				result.FullTypeName = currentFullTypeName.c_str();
				return result;
			}
		}

		result.Success = false;
		return result;
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