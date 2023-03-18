#pragma once

#include "EditorProperty.h"

/// A function with the system function marked with this macro signature will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked function into TypeRegistry as a system. \n\n
/// Example:\n
/// DYE_SYSTEM_FUNCTION("SystemFoo", SystemFunctionFoo) \n
/// void SystemFunctionFoo(DYE::DYEntity::World& world) { //... } \n\n
/// Notice that the signature of the function should be void (World&), otherwise the function is invalid as a system.
/// \param systemName the string name that will be used to identify the system in DYEditor.
/// \param systemFunction the system function. You should include namespace(s) as if the function is being accessed in global scope.
/// \param optionalParameters ...to define custom draw function etc
#define DYE_SYSTEM_FUNCTION(systemName, systemFunction, ...)

#define DYE_SYSTEM_FUNCTION_SIGNATURE(functionName) void functionName(DYE::DYEntity::World& world)

/// A class marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class into TypeRegistry as a system. \n\n
/// Example:\n
/// DYE_SYSTEM("SystemFoo", SystemFunctionFoo) \n
/// class SystemFoo final : public SystemBase { //... } \n\n
/// Notice that the class should always be derived from SystemBase class.
/// \param systemName the string name that will be used to identify the system in DYEditor.
/// \param systemType the system type. You should include namespace(s) as if the type is being accessed in global scope.
/// \param optionalParameters ...to define tooltip etc
#define DYE_SYSTEM(systemName, systemType, ...)