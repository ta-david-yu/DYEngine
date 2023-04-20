#pragma once

#include "Core/Entity.h"
#include "Type/DefaultComponentFunctions.h"

#include <string>
#include <vector>
#include <optional>
#include <map>

namespace DYE::DYEditor
{
	class SystemBase;

	struct SerializedEntity;
	struct SerializedComponentHandle;

	struct SerializationResult
	{
		bool Success = true;
	};

	struct DeserializationResult
	{
		bool Success = true;
	};

	using HasComponentFunction = bool (DYE::DYEditor::Entity& entity);
	using AddComponentFunction = void (DYE::DYEditor::Entity& entity);
	using RemoveComponentFunction = void (DYE::DYEditor::Entity& entity);
	/// Serialize a component on an entity to a serialized entity.
	using SerializeComponentFunction = SerializationResult (DYE::DYEditor::Entity& entity, SerializedComponentHandle& serializedComponent);
	/// Deserialize a serialized component (handle) and add it to an entity.
	using DeserializeComponentFunction = DeserializationResult (SerializedComponentHandle& serializedComponent, DYE::DYEditor::Entity& entity);
	/// \return true if the content of the inspector is changed/dirty.
	using DrawComponentInspectorFunction = bool (DYE::DYEditor::Entity& entity);
	/// \return true if the content of the inspector is changed/dirty.
	using DrawComponentHeaderFunction = bool (DYE::DYEditor::Entity& entity, bool &isHeaderVisible, bool &entityChanged, std::string const& headerLabel);

	/// One should always provide 'Serialize', 'Deserialize' and 'DrawInspector' functions.
	struct ComponentTypeDescriptor
	{
		HasComponentFunction* Has = nullptr;
		AddComponentFunction* Add = nullptr;
		RemoveComponentFunction* Remove = nullptr;

		SerializeComponentFunction* Serialize = nullptr;
		DeserializeComponentFunction* Deserialize = nullptr;

		bool ShouldDrawInNormalInspector = true;
		DrawComponentInspectorFunction* DrawInspector = nullptr;
		DrawComponentHeaderFunction* DrawHeader = nullptr;
	};

	// TypeRegistry keeps track of all the types, so we could use them in runtime.
	// Types including built-in & user-defined components, systems, levels etc.
	class TypeRegistry
	{
	public:
		/// Register a component type with its corresponding editor utility functions.
		/// \param descriptor One could simply use the trivial function implementations by assigning null function pointer to the target function.
		/// For now only 'Has', 'Add', 'Remove' have default implementations that make sense. For other functions, it's
		/// necessary to assign user-defined functions.
		template<typename T>
		static void RegisterComponentType(std::string const &componentName, ComponentTypeDescriptor descriptor)
		{
			if (descriptor.Has == nullptr)
			{
				descriptor.Has = DefaultHasComponentOfType<T>;
			}

			if (descriptor.Add == nullptr)
			{
				descriptor.Add = DefaultAddComponentOfType<T>;
			}

			if (descriptor.Remove == nullptr)
			{
				descriptor.Remove = DefaultRemoveComponentOfType<T>;
			}

			registerComponentType(componentName, descriptor);
		}

		static void ClearRegisteredComponentTypes();
		static void ClearRegisteredSystems();

		/// Retrieves an array of pairs containing information about registered components.
		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::pair<std::string, ComponentTypeDescriptor>> GetComponentTypesNamesAndDescriptors();

		static void RegisterSystem(std::string const& systemName, SystemBase* systemInstance);

		/// Retrieves an array of pairs of system names and instances.
		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::pair<std::string, SystemBase*>> GetSystemNamesAndInstances();

		static std::optional<ComponentTypeDescriptor> TryGetComponentTypeDescriptor(std::string const& componentTypeName);

		/// \return a pointer to the system instance, else nullptr.
		static SystemBase* TryGetSystemInstance(std::string const& systemName);

	private:
		static void registerComponentType(std::string const &componentName, ComponentTypeDescriptor componentDescriptor);

	private:
		// TODO: maybe we could use array or vector instead?
		inline static std::map<std::string, ComponentTypeDescriptor> s_ComponentTypeRegistry;
		inline static std::map<std::string, SystemBase*> s_SystemRegistry;

		static std::vector<std::pair<std::string, ComponentTypeDescriptor>> s_ComponentNamesAndDescriptorsCache;
		static std::vector<std::pair<std::string, SystemBase*>> s_SystemNamesAndPointersCache;
	};
}