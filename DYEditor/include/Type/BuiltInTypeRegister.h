#pragma once

#include <string>

#include "Util/Macro.h"

namespace DYE::DYEditor
{
	char const *const NameComponentTypeName = NAME_OF(DYE::DYEditor::NameComponent);
	char const *const LocalTransformComponentTypeName = NAME_OF(DYE::DYEditor::LocalTransform);
	char const *const ParentComponentTypeName = NAME_OF(DYE::DYEditor::Parent);
	char const *const ChildrenComponentTypeName = NAME_OF(DYE::DYEditor::Children);

	void RegisterBuiltInTypes();
}