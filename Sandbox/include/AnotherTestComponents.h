#pragma once

#include "SerializableType.h"

DYE_COMPONENT("TestC", TestComponentC)
struct TestComponentC
{
	DYE_PROPERTY()
	DYE::Color4 ColorValue;
};