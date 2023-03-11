#pragma once

#include "SerializableType.h"

namespace TestNamespace
{
	DYE_COMPONENT("TestA", TestNamespace::TestComponents)
	struct TestComponents
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
	DYE::Float FloatValue;
};
