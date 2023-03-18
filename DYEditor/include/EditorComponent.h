#pragma once

#include "EditorProperty.h"

/// A class/struct marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class/struct into TypeRegistry as a component.\n\n
/// Example:\n
/// DYE_COMPONENT("Foo", FooComponent) \n
/// struct FooComponent { } \n\n
/// \param componentName the string name that will be used to identify the component type in DYEditor.
/// \param componentType the component type. You should include namespace(s) as if the type is being accessed in global scope.
/// \param optionalParameters ...to define custom draw function, tooltip etc
#define DYE_COMPONENT(componentName, componentType, ...)
