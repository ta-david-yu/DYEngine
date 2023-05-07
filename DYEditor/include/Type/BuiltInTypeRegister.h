#pragma once

#include <string>

namespace DYE::DYEditor
{
	const std::string NameComponentName = "Name";
	const std::string TransformComponentName = "Transform";
	const std::string ParentComponentName = "Parent";
	const std::string ChildrenComponentName = "Children";

	void RegisterBuiltInTypes();
}