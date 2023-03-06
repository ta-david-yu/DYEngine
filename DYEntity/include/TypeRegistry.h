#pragma once

#include "Entity.h"

#include <string>
#include <vector>
#include <optional>
#include <map>

namespace DYE::DYEntity
{
	class Stream;

	using HasComponentFunction = bool (Entity& entity);
	using AddComponentFunction = void (Entity& entity);
	using RemoveComponentFunction = void (Entity& entity);
	using SerializeComponentFunction = void (Entity& entity, Stream& streamToSerializeTo);
	using DeserializeComponentFunction = void (Stream& streamToDeserializeFrom, Entity& entity);

	/// \return true if the content of the inspector is changed/dirty.
	using DrawInspectorFunction = bool (Entity& entity);

	struct ComponentTypeFunctionCollection
	{
		HasComponentFunction* Has = nullptr;
		AddComponentFunction* Add = nullptr;
		RemoveComponentFunction* Remove = nullptr;

		SerializeComponentFunction* Serialize = nullptr;
		DeserializeComponentFunction* Deserialize = nullptr;
		DrawInspectorFunction* DrawInspector = nullptr;
	};

	template<typename T>
	bool DefaultHasComponentOfType(Entity& entity)
	{
		return entity.HasComponent<T>();
	}

	template<typename T>
	void DefaultAddComponentOfType(Entity& entity)
	{
		// By default, add component to the entity using default constructor
		entity.AddComponent<T>();
	}

	template<typename T>
	void DefaultRemoveComponentOfType(Entity& entity)
	{
		entity.RemoveComponent<T>();
	}

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

		/// Retrieves an array of pairs containing information about registered components.
		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> GetComponentTypesNamesAndFunctionCollections();

		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::string> GetComponentTypeNames();

		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<ComponentTypeFunctionCollection> GetComponentTypeFunctionCollections();
		static std::optional<ComponentTypeFunctionCollection> TryGetComponentTypeFunctionsFromName(std::string const& componentName);

	private:
		static void registerComponentType(std::string const &componentName, ComponentTypeFunctionCollection functions);

	private:
		inline static std::map<std::string, ComponentTypeFunctionCollection> s_ComponentTypeRegistry;
	};
}