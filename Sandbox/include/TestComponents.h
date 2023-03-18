#pragma once

#include "EditorCore.h"

namespace TestNamespace
{
	DYE_COMPONENT("TestA", TestNamespace::TestComponentA)
	struct TestComponentA
	{
		DYE_PROPERTY()
		DYE::Float FloatValue;

		DYE_PROPERTY()
		DYE::Int32 IntegerValue;

		DYE_PROPERTY()
		int intCannotBeSerialized;

		DYE::Int32 DYEIntNotSerialized;

		int intNotSerialized;
	};
}

DYE_COMPONENT("TestB", TestComponentB)
struct TestComponentB
{
	DYE_PROPERTY()
	DYE::Bool BooleanValue;

	DYE_PROPERTY()
	DYE::Char OneCharacter = 'a';

	DYE_PROPERTY()
	const DYE::Float ConstantFloat;

	DYE_PROPERTY()
	const DYE::Vector3 ConstantVector3;

	DYE_PROPERTY()
	DYE::Vector3 Position = glm::vec3 {0, 0, 5};

	DYE_PROPERTY()
	DYE::Vector4 vec4 = glm::vec4 {1, 2, 3, 4};
};
