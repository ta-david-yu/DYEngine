#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#define DYE_STRING(type) #type

/// A class/struct marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class/struct into TypeRegistry as a component.
/// Example:
/// DYE_COMPONENT("Foo", FooComponent)
/// struct FooComponent { }
/// \param componentName the string name that will be used to identify the component type.
/// \param componentType the component type. You should include namespace(s) as if the type is being accessed in global scope.
#define DYE_COMPONENT(componentName, componentType)

namespace DYE::DYEditor
{
	using Char = char;
	using Bool = bool;
	using Int8 = std::int8_t;
	using Int16 = std::int16_t;
	using Int32 = std::int32_t;
	using Int64 = std::int64_t;
	using Uint8 = std::uint8_t;
	using Uint16 = std::uint16_t;
	using Uint32 = std::uint32_t;
	using Uint64 = std::uint64_t;
	using Float = float;
	using Double = double;
	using Vector2 = glm::vec3;
	using Vector3 = glm::vec3;
	using Vector4 = glm::vec4;
	using Color4 = glm::vec4;
	using String = std::string;

	// TODO: need to figure out a way to handle enum type
	//		 maybe we can let user provide a specialized template function that
	//       [de]serialize + draw imgui for an enum property type?
	template<typename E>
	concept EnumTypeName = std::is_enum_v<E>;

	// Make an explicit specialization of this template function to support an enum type with DYEditor
	// code generation system. The template function specialization should return a table of enumerator -> enumerator name string.
	//
	// Example:
	//
	//	enum class Fruit
	//	{
	//		Apple, Banana, Orange
	//	};
	//
	//	template<>
	//	std::vector<std::pair<Fruit, std::string>> GetEnumTypeStringTable()
	//	{
	//		return {{ Fruit::Apple, STRING(Apple) }, { Fruit::Banana, STRING(Banana) }, { Fruit::Orange, STRING(Orange) }};
	//	}
	//
	template<EnumTypeName EnumType>
	std::vector<std::pair<EnumType, std::string>> GetEnumTypeStringTable()
	{
		// By default, it returns an empty vector.
		// Therefore, none of the enumerators in the given enum type will be included in the
		// auto-generated [de]serialization & imgui-draw functions.
		return {};
	}
}
