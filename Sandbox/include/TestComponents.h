#pragma once

#include "SerializableType.h"

namespace TestNamespace
{
	DYE_COMPONENT("TestA", TestNamespace::TestComponentA)
	struct TestComponentA
	{
		DYE_PROPERTY()
		DYE::Float FloatValue;

		DYE_PROPERTY()
		DYE::Int32 IntegerValue;
	};
}

DYE_COMPONENT("TestB", TestComponentB)
struct TestComponentB
{
	DYE_PROPERTY()
	DYE::Bool BooleanValue;

	DYE_PROPERTY()
	DYE::Char OneCharacter;

	DYE_PROPERTY()
	const DYE::Float FloatValue;

	DYE_PROPERTY()
	DYE::Vector3 Position = glm::vec3 {0, 0, 5};

	DYE_PROPERTY()
	DYE::Vector4 vec4 = glm::vec4 {1, 2, 3, 4};

	DYE_PROPERTY()
	int intVal;
};
