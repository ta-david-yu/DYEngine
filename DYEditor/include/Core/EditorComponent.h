#pragma once

#include "Core/EditorProperty.h"

/// A class/struct marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class/struct into TypeRegistry as a component.\n\n
///
/// Usage Example:\n
/// DYE_COMPONENT("Foo", FooComponent) \n
/// struct FooComponent { }
///
/// \param componentName the string name that will be used to identify the component type in DYEditor.
/// \param componentType the component type. You should include namespace(s) as if the type is being accessed in global scope.
/// \param optionalParameters ...to define custom draw function, tooltip etc
#define DYE_COMPONENT(componentName, componentType, ...)

// TODO: redesign DYE_COMPONENT syntax
//
// DYE_COMPONENT(componentType, optionalDisplayName, ...)
// \param componentType the same as before, but will also be used as the id (key) to the component type descriptor.
// \param optionalDisplayName if specified, whenever the component type is to be referred to in an user interface, optionalDisplayName will be shown instead. This should be a string literal.
//
// Example:
// 	DYE_COMPONENT(FooComponent, "Foo")
// 	struct FooComponent { }

// TODO: a bunch of additional component helper macros

/// Similar to FormerlySerializedAsAttribute in Unity. String literals.
#define DYE_FORMERLY_KNOWN_AS(componentTypeNames...)

/// The class/struct type which has been marked with DYE_COMPONENT and immediately followed by this macro will keep track of a list of additional components
/// which should be used together on the same entity, in order for it to take effect.
/// For instance, to be rendered by the render system, an entity with a renderer component should also have a transform component.\n\n
///
/// Usage Example:\n
/// DYE_COMPONENT(DYE::DYEditor::MeshRenderer, "MeshRenderer") \n
/// struct MeshRenderer { } \n\n
///
/// As the syntax merely hints/suggests the accompany component types,
/// the engine does not guarantee the hinted component types will always be added the same entity which has the current DYE_COMPONENT component type.
///
/// \param componentTypes... the list of component types that should be used along with the current DYE_COMPONENT component.
///
#define DYE_ADD_COMPONENT_HINT(componentTypes...)
