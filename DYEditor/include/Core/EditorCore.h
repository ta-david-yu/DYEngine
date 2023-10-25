#pragma once

#include "Core/EditorProperty.h"
#include "Core/EditorComponent.h"
#include "Core/EditorSystem.h"

/// Similar to FormerlySerializedAsAttribute in Unity, but on DYE types (i.e., DYE_COMPONENT, DYE_SYSTEM). \n\n
///
/// Usage Example:\n
/// DYE_COMPONENT(DYE::DYEditor::NameComponent, "Name") \n
/// DYE_FORMERLY_KNOWN_AS("Name") \n
/// struct NameComponent { } \n\n
///
/// \param typeName formerly known type name. It should be a string literal (i.e., "TransformComponent"). Be aware it's 'type name' not 'display name'
///
#define DYE_FORMERLY_KNOWN_AS(typeName)
