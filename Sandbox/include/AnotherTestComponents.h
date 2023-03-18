#pragma once

#include "EditorCore.h"
#include "Math/Color.h"

DYE_COMPONENT("TestC", TestComponentC)
struct TestComponentC
{
	DYE_PROPERTY()
	DYE::Color4 ColorValue = DYE::Color::Yellow;

	DYE_PROPERTY()
	DYE::String TestName = "WHY IS IT LIKE THAT?!";
};