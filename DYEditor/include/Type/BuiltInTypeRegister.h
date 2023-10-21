#pragma once

#include <string>

namespace DYE::DYEditor
{
	char const *const NameComponentName = "Name";
	char const *const TransformComponentName = "Transform";
	char const *const ParentComponentName = "Parent";
	char const *const ChildrenComponentName = "Children";

	void RegisterBuiltInTypes();
}