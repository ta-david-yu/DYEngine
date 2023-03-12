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

	DYE_PROPERTY()
	DYE::Bool const BooleanValue;

	DYE_PROPERTY()
	const DYE::Float FloatValue;
};
