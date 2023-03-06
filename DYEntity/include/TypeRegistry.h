#pragma once

#include <string>
#include <vector>
#include <optional>
#include <map>

namespace DYE::DYEntity
{
	class World;
	class Entity;
	class Stream;

	using HasComponentFunction = bool (Entity& entity);
	using AddComponentFunction = void (Entity& entity);
	using SerializeComponentFunction = void (Entity& entity, Stream& streamToSerializeTo);
	using DeserializeComponentFunction = void (Stream& streamToDeserializeFrom, Entity& entity);
	using DrawInspectorFunction = bool (Entity& entity);

	struct ComponentTypeFunctionCollection
	{
		HasComponentFunction* Has = nullptr;
		AddComponentFunction* Add = nullptr;
		SerializeComponentFunction* Serialize = nullptr;
		DeserializeComponentFunction* Deserialize = nullptr;
		DrawInspectorFunction* DrawInspector = nullptr;
	};

	// TypeRegistry keeps track of all the types, so we could use them in runtime.
	// Types including built-in & user-defined components, systems, levels etc.
	class TypeRegistry
	{
	public:
		static void RegisterComponentType(std::string const &componentName, ComponentTypeFunctionCollection functions);

		/// Retrieves an array of pairs containing information about registered components.
		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> GetComponentTypesNamesAndFunctionCollections();

		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<std::string> GetComponentTypeNames();

		/// The function is expensive, the user should cache the result instead of calling the function regularly.
		static std::vector<ComponentTypeFunctionCollection> GetComponentTypeFunctionCollections();
		static std::optional<ComponentTypeFunctionCollection> TryGetComponentTypeFunctionsFromName(std::string const& componentName);

	private:
		inline static std::map<std::string, ComponentTypeFunctionCollection> s_ComponentTypeRegistry;
	};
}