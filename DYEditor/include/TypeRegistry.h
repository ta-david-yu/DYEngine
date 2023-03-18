#pragma once

#include "Entity.h"
#include "DefaultComponentFunctions.h"

#include <string>
#include <vector>
#include <optional>
#include <map>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	class Stream;

	using HasComponentFunction = bool (Entity& entity);
	using AddComponentFunction = void (Entity& entity);
	using RemoveComponentFunction = void (Entity& entity);
	using SerializeComponentFunction = void (Entity& entity, Stream& streamToSerializeTo);
	using DeserializeComponentFunction = void (Stream& streamToDeserializeFrom, Entity& entity);
	/// \return true if the content of the inspector is changed/dirty.
	using DrawComponentInspectorFunction = bool (Entity& entity);

	struct ComponentTypeFunctionCollection
	{
		HasComponentFunction* Has = nullptr;
		AddComponentFunction* Add = nullptr;
		RemoveComponentFunction* Remove = nullptr;

		SerializeComponentFunction* Serialize = nullptr;
		DeserializeComponentFunction* Deserialize = nullptr;
		DrawComponentInspectorFunction* DrawInspector = nullptr;
	};

	using SystemFunction = void (World& world);
	using DrawSystemInspectorFunction = void (World& world);

	struct SystemFunctionCollection
	{

	};

	// TypeRegistry keeps track of all the types, so we could use them in runtime.
	// Types including built-in & user-defined components, systems, levels etc.
	class TypeRegistry
	{
	public:

		/// Register a component type with its corresponding editor utility functions.
		/// \param functions One could simply use the trivial function implementations by assigning null function pointer to the target function.
		/// For now only 'Has', 'Add', 'Remove' have default implementations that make sense. For other functions, it's
		/// necessary to assign user-defined functions.
		template<typename T>
		static void RegisterComponentType(std::string const &componentName, ComponentTypeFunctionCollection functions)
		{
			if (functions.Has == nullptr)
			{
				functions.Has = DefaultHasComponentOfType<T>;
			}

			if (functions.Add == nullptr)
			{
				functions.Add = DefaultAddComponentOfType<T>;
			}

			if (functions.Remove == nullptr)
			{
				functions.Remove = DefaultRemoveComponentOfType<T>;
			}

			registerComponentType(componentName, functions);
		}

		static void ClearRegisteredComponentTypes();

		/// Retrieves an array of pairs containing information about registered components.
		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> GetComponentTypesNamesAndFunctionCollections();

		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::string> GetComponentTypeNames();

		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<ComponentTypeFunctionCollection> GetComponentTypeFunctionCollections();
		static std::optional<ComponentTypeFunctionCollection> TryGetComponentTypeFunctionsFromName(std::string const& componentName);

		static void RegisterSystemFunction(std::string const &systemName, SystemFunction* systemFunction);

		/// Retrieves an array of pairs containing information about registered components.
		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::pair<std::string, SystemFunction*>> GetSystemNamesAndFunctions();

	private:
		static void registerComponentType(std::string const &componentName, ComponentTypeFunctionCollection functions);

	private:
		// TODO: maybe we could use array or vector instead?
		inline static std::map<std::string, ComponentTypeFunctionCollection> s_ComponentTypeRegistry;
		inline static std::map<std::string, SystemFunction*> s_SystemFunctionRegistry;
	};
}