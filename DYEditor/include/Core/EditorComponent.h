#pragma once

#include "Core/EditorProperty.h"

/// A class/struct marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class/struct into TypeRegistry as a component.\n\n
///
/// Usage Example:
/// 	DYE_COMPONENT(FooComponent, "Foo")
/// 	struct FooComponent { }
///
/// \param componentType the same as before, but will also be used as the id (key) to the component type descriptor.
/// \param optionalDisplayName if specified, whenever the component type is to be referred to in an user interface, optionalDisplayName will be shown instead. This should be a string literal.
///
#define DYE_COMPONENT(componentType, optionalDisplayName...)

/// The class/struct type which has been marked with DYE_COMPONENT and immediately followed by this macro will keep track of an additional component
/// which should be used together on the same entity, in order for it to take effect.
/// For instance, to be rendered by the render system, an entity with a renderer component should also have a transform component.\n\n
///
/// Usage Example:\n
/// DYE_COMPONENT(DYE::DYEditor::MeshRenderer, "MeshRenderer") \n
/// DYE_USE_WITH_COMPONENT_HINT(DYE::DYEditor::LocalTransform) \n
/// DYE_USE_WITH_COMPONENT_HINT(DYE::DYEditor::MeshFilter) \n
/// struct MeshRenderer { } \n\n
///
/// As the syntax merely hints/suggests the accompany component types,
/// the engine does not guarantee the hinted component types will always be added the same entity which has the current DYE_COMPONENT component type.
///
/// \param componentType the component type that should be used along with the current DYE_COMPONENT component.
///
#define DYE_USE_WITH_COMPONENT_HINT(componentType)
