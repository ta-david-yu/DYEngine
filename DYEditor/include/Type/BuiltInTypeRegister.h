#pragma once

#include <string>

namespace DYE::DYEditor
{
	char const *const NameComponentName = "Name";
	char const *const LocalTransformComponentName = "LocalTransform";
	char const *const ParentComponentName = "Parent";
	char const *const ChildrenComponentName = "Children";

	void RegisterBuiltInTypes();
}