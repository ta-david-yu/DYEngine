#pragma once

#include "Core/EditorCore.h"
#include "Math/Color.h"

DYE_COMPONENT("TestC", TestComponentC)
struct TestComponentC
{
	DYE_PROPERTY()
	DYE::Color4 ColorValue = DYE::Color::Yellow;

	DYE_PROPERTY()
	DYE::Char TestChar2 = '2';


	DYE_PROPERTY()
	DYE::String TestName = "WHY IS IT LIKE THAT?!";

	DYE_PROPERTY()
	DYE::Char TestChar = 'X';
};