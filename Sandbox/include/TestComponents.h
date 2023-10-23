#pragma once

#include "Core/EditorCore.h"


namespace TestNamespace
{
	DYE_COMPONENT(TestNamespace::TestComponentA, "TestA")
	struct TestComponentA
	{
		DYE_PROPERTY()
		DYE::Float FloatValue=1.0f;

		DYE_PROPERTY()
		DYE::Int32 IntegerValue = 20;

		DYE_PROPERTY()
		int intCannotBeSerialized;

		DYE::Int32 DYEIntNotSerialized;

		int intNotSerialized;
	};

	namespace Subnamespace
	{
		DYE_COMPONENT(TestNamespace::Subnamespace::SubtestComponentA)
		struct SubtestComponentA
		{
			DYE_PROPERTY()
			DYE::Int32 IntegerValue;
		};
	}
}

DYE_COMPONENT(TestComponentB)
struct TestComponentB
{
	DYE_PROPERTY()
	DYE::Bool BooleanValue;

	DYE_PROPERTY()
	DYE::Char OneCharacter = 'a';

	DYE_PROPERTY()
	const DYE::Float ConstantFloat;

	DYE_PROPERTY()
	const DYE::Vector3 ConstantVector3 {0, 0, 0};

	DYE_PROPERTY()
	DYE::Vector3 Position = glm::vec3 {0, 0, 5};

	DYE_PROPERTY()
	DYE::Vector4 vec4 = glm::vec4 {1, 2, 3, 4};
};

DYE_COMPONENT(ComponentWithAllPrimitiveProperties, "ComponentWithAllPrimitiveProperties")
struct ComponentWithAllPrimitiveProperties
{
	DYE_PROPERTY()
	DYE::Char CharVar = 'a';

	DYE_PROPERTY()
	DYE::Bool BoolVar = false;

	DYE_PROPERTY()
	DYE::Int32 Int32Var;

//	DYE_PROPERTY()
//	DYE::Int64 Int64Var;
//
//	DYE_PROPERTY()
//	DYE::Uint8 Uint8Var;
//
//	DYE_PROPERTY()
//	DYE::Uint16 Uint16Var;
//
//	DYE_PROPERTY()
//	DYE::Uint32 Uint32Var;
//
//	DYE_PROPERTY()
//	DYE::Uint64 Uint64Var;

	DYE_PROPERTY()
	DYE::Float FloatVar;

//	DYE_PROPERTY()
//	DYE::Double DoubleVar;

	DYE_PROPERTY()
	DYE::Vector2 Vector2Var;

	DYE_PROPERTY()
	DYE::Vector3 Vector3Var;

	DYE_PROPERTY()
	DYE::Vector4 Vector4Var;

	DYE_PROPERTY()
	DYE::Color4 Color4Var;

	DYE_PROPERTY()
	DYE::String StringVar;

	DYE_PROPERTY()
	DYE::Quaternion QuaternionVar;
};


DYE_COMPONENT(HasAngularVelocity)
struct HasAngularVelocity
{
	DYE_PROPERTY()
	DYE::Float AngleDegreePerSecond = 30.0f;
};

DYE_COMPONENT(CreateEntity)
struct CreateEntity
{
	DYE_PROPERTY()
	DYE::String EntityNamePrefix;

	DYE_PROPERTY()
	DYE::Int32 NumberOfEntitiesToCreate = 10;
};

DYE_COMPONENT(PrintMessageOnTeardown)
struct PrintMessageOnTeardown
{
	DYE_PROPERTY()
	DYE::String Message = "";
};