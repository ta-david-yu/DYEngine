//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by DYEditorCodeGenerator.
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
#include "Type/UserTypeRegister.h"

#include "Type/TypeRegistry.h"
#include "Serialization/SerializedObjectFactory.h"
#include "ImGui/ImGuiUtil.h"

// Insert user headers here...
#include "include/TestComponents.h"
#include "include/AnotherTestComponents.h"
#include "include/SystemExample.h"

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	void userRegisterTypeFunction()
	{
		// Insert user type registration here...

		// Component located in include/TestComponents.h
		TypeRegistry::RegisterComponentType<TestNamespace::TestComponentA>
			(
				"TestA",
				ComponentTypeFunctionCollection
					{
						.Serialize = [](Entity& entity, SerializedComponentHandle& serializedComponent)
						{
							auto const& component = entity.GetComponent<TestNamespace::TestComponentA>();
							serializedComponent.SetPrimitiveTypePropertyValue("FloatValue", component.FloatValue);
							serializedComponent.SetPrimitiveTypePropertyValue("IntegerValue", component.IntegerValue);
							// Property 'intCannotBeSerialized' will not be serialized because its type 'int' is not supported.
							return SerializationResult {};
						},
						.Deserialize = [](SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
						{
							auto& component = entity.AddOrGetComponent<TestNamespace::TestComponentA>();
							component.FloatValue = serializedComponent.GetPrimitiveTypePropertyValueOr<Float>("FloatValue", 1.0f);
							component.IntegerValue = serializedComponent.GetPrimitiveTypePropertyValueOr<Int32>("IntegerValue", 20);
							// Property 'intCannotBeSerialized' will not be serialized because its type 'int' is not supported.
							return DeserializationResult {};
						},
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							auto& component = entity.GetComponent<TestNamespace::TestComponentA>();
							ImGui::TextWrapped("TestNamespace::TestComponentA");
							changed |= ImGuiUtil::DrawFloatControl("FloatValue", component.FloatValue);
							changed |= ImGuiUtil::DrawIntControl("IntegerValue", component.IntegerValue);
							ImGui::BeginDisabled(true); ImGuiUtil::DrawReadOnlyTextWithLabel("intCannotBeSerialized", "Variable of unsupported type 'int'"); ImGui::EndDisabled();
							return changed;
						}
					}
			);

		// Component located in include/TestComponents.h
		TypeRegistry::RegisterComponentType<TestNamespace::Subnamespace::SubtestComponentA>
			(
				"SubTestA",
				ComponentTypeFunctionCollection
					{
						.Serialize = [](Entity& entity, SerializedComponentHandle& serializedComponent)
						{
							auto const& component = entity.GetComponent<TestNamespace::Subnamespace::SubtestComponentA>();
							serializedComponent.SetPrimitiveTypePropertyValue("IntegerValue", component.IntegerValue);
							return SerializationResult {};
						},
						.Deserialize = [](SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
						{
							auto& component = entity.AddOrGetComponent<TestNamespace::Subnamespace::SubtestComponentA>();
							component.IntegerValue = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Int32>("IntegerValue");
							return DeserializationResult {};
						},
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							auto& component = entity.GetComponent<TestNamespace::Subnamespace::SubtestComponentA>();
							ImGui::TextWrapped("TestNamespace::Subnamespace::SubtestComponentA");
							changed |= ImGuiUtil::DrawIntControl("IntegerValue", component.IntegerValue);
							return changed;
						}
					}
			);

		// Component located in include/TestComponents.h
		TypeRegistry::RegisterComponentType<TestComponentB>
			(
				"TestB",
				ComponentTypeFunctionCollection
					{
						.Serialize = [](Entity& entity, SerializedComponentHandle& serializedComponent)
						{
							auto const& component = entity.GetComponent<TestComponentB>();
							serializedComponent.SetPrimitiveTypePropertyValue("BooleanValue", component.BooleanValue);
							std::string OneCharacter(" "); OneCharacter[0] = component.OneCharacter; serializedComponent.SetPrimitiveTypePropertyValue("OneCharacter", OneCharacter);							// Property 'ConstantFloat' will not be serialized because it is a constant variable.
							// Property 'ConstantVector3' will not be serialized because it is a constant variable.
							serializedComponent.SetPrimitiveTypePropertyValue("Position", component.Position);
							serializedComponent.SetPrimitiveTypePropertyValue("vec4", component.vec4);
							return SerializationResult {};
						},
						.Deserialize = [](SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
						{
							auto& component = entity.AddOrGetComponent<TestComponentB>();
							component.BooleanValue = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Bool>("BooleanValue");
							component.OneCharacter = serializedComponent.GetPrimitiveTypePropertyValueOr<const char*>("OneCharacter", "a")[0];
							// Property 'ConstantFloat' will not be serialized because it is a constant variable.
							// Property 'ConstantVector3' will not be serialized because it is a constant variable.
							component.Position = serializedComponent.GetPrimitiveTypePropertyValueOr<Vector3>("Position", glm::vec3 {0, 0, 5});
							component.vec4 = serializedComponent.GetPrimitiveTypePropertyValueOr<Vector4>("vec4", glm::vec4 {1, 2, 3, 4});
							return DeserializationResult {};
						},
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							auto& component = entity.GetComponent<TestComponentB>();
							ImGui::TextWrapped("TestComponentB");
							changed |= ImGuiUtil::DrawBoolControl("BooleanValue", component.BooleanValue);
							changed |= ImGuiUtil::DrawCharControl("OneCharacter", component.OneCharacter);
							ImGui::BeginDisabled(true); ImGuiUtil::DrawReadOnlyTextWithLabel("ConstantFloat", "Constant variable of type 'Float'"); ImGui::EndDisabled();
							ImGui::BeginDisabled(true); ImGuiUtil::DrawReadOnlyTextWithLabel("ConstantVector3", "Constant variable of type 'Vector3'"); ImGui::EndDisabled();
							changed |= ImGuiUtil::DrawVector3Control("Position", component.Position);
							changed |= ImGuiUtil::DrawVector4Control("vec4", component.vec4);
							return changed;
						}
					}
			);

		// Component located in include/TestComponents.h
		TypeRegistry::RegisterComponentType<ComponentWithAllPrimitiveProperties>
			(
				"ComponentWithAllPrimitiveProperties",
				ComponentTypeFunctionCollection
					{
						.Serialize = [](Entity& entity, SerializedComponentHandle& serializedComponent)
						{
							auto const& component = entity.GetComponent<ComponentWithAllPrimitiveProperties>();
							std::string CharVar(" "); CharVar[0] = component.CharVar; serializedComponent.SetPrimitiveTypePropertyValue("CharVar", CharVar);							serializedComponent.SetPrimitiveTypePropertyValue("BoolVar", component.BoolVar);
							serializedComponent.SetPrimitiveTypePropertyValue("Int32Var", component.Int32Var);
							serializedComponent.SetPrimitiveTypePropertyValue("FloatVar", component.FloatVar);
							serializedComponent.SetPrimitiveTypePropertyValue("Vector2Var", component.Vector2Var);
							serializedComponent.SetPrimitiveTypePropertyValue("Vector3Var", component.Vector3Var);
							serializedComponent.SetPrimitiveTypePropertyValue("Vector4Var", component.Vector4Var);
							serializedComponent.SetPrimitiveTypePropertyValue("Color4Var", component.Color4Var);
							serializedComponent.SetPrimitiveTypePropertyValue("StringVar", component.StringVar);
							serializedComponent.SetPrimitiveTypePropertyValue("QuaternionVar", component.QuaternionVar);
							return SerializationResult {};
						},
						.Deserialize = [](SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
						{
							auto& component = entity.AddOrGetComponent<ComponentWithAllPrimitiveProperties>();
							component.CharVar = serializedComponent.GetPrimitiveTypePropertyValueOr<const char*>("CharVar", "a")[0];
							component.BoolVar = serializedComponent.GetPrimitiveTypePropertyValueOr<Bool>("BoolVar", false);
							component.Int32Var = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Int32>("Int32Var");
							component.FloatVar = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Float>("FloatVar");
							component.Vector2Var = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Vector2>("Vector2Var");
							component.Vector3Var = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Vector3>("Vector3Var");
							component.Vector4Var = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Vector4>("Vector4Var");
							component.Color4Var = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Color4>("Color4Var");
							component.StringVar = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<String>("StringVar");
							component.QuaternionVar = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<Quaternion>("QuaternionVar");
							return DeserializationResult {};
						},
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							auto& component = entity.GetComponent<ComponentWithAllPrimitiveProperties>();
							ImGui::TextWrapped("ComponentWithAllPrimitiveProperties");
							changed |= ImGuiUtil::DrawCharControl("CharVar", component.CharVar);
							changed |= ImGuiUtil::DrawBoolControl("BoolVar", component.BoolVar);
							changed |= ImGuiUtil::DrawIntControl("Int32Var", component.Int32Var);
							changed |= ImGuiUtil::DrawFloatControl("FloatVar", component.FloatVar);
							changed |= ImGuiUtil::DrawVector2Control("Vector2Var", component.Vector2Var);
							changed |= ImGuiUtil::DrawVector3Control("Vector3Var", component.Vector3Var);
							changed |= ImGuiUtil::DrawVector4Control("Vector4Var", component.Vector4Var);
							changed |= ImGuiUtil::DrawColor4Control("Color4Var", component.Color4Var);
							changed |= ImGuiUtil::DrawTextControl("StringVar", component.StringVar);
							// 'QuaternionVar' : Quaternion 
							{
								glm::vec3 eulerDegree = glm::eulerAngles(component.QuaternionVar);
								eulerDegree += glm::vec3(0.f); eulerDegree = glm::degrees(eulerDegree);
								if (ImGuiUtil::DrawVector3Control("QuaternionVar", eulerDegree))
								{
									eulerDegree.y = glm::clamp(eulerDegree.y, -90.f, 90.f);
									component.QuaternionVar = glm::quat (glm::radians(eulerDegree));
									changed = true;
								}
							}
							return changed;
						}
					}
			);

		// Component located in include/TestComponents.h
		TypeRegistry::RegisterComponentType<HasAngularVelocity>
			(
				"HasAngularVelocity",
				ComponentTypeFunctionCollection
					{
						.Serialize = [](Entity& entity, SerializedComponentHandle& serializedComponent)
						{
							auto const& component = entity.GetComponent<HasAngularVelocity>();
							serializedComponent.SetPrimitiveTypePropertyValue("AngleDegreePerSecond", component.AngleDegreePerSecond);
							return SerializationResult {};
						},
						.Deserialize = [](SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
						{
							auto& component = entity.AddOrGetComponent<HasAngularVelocity>();
							component.AngleDegreePerSecond = serializedComponent.GetPrimitiveTypePropertyValueOr<Float>("AngleDegreePerSecond", 30.0f);
							return DeserializationResult {};
						},
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							auto& component = entity.GetComponent<HasAngularVelocity>();
							ImGui::TextWrapped("HasAngularVelocity");
							changed |= ImGuiUtil::DrawFloatControl("AngleDegreePerSecond", component.AngleDegreePerSecond);
							return changed;
						}
					}
			);

		// Component located in include/AnotherTestComponents.h
		TypeRegistry::RegisterComponentType<TestComponentC>
			(
				"TestC",
				ComponentTypeFunctionCollection
					{
						.Serialize = [](Entity& entity, SerializedComponentHandle& serializedComponent)
						{
							auto const& component = entity.GetComponent<TestComponentC>();
							serializedComponent.SetPrimitiveTypePropertyValue("ColorValue", component.ColorValue);
							std::string TestChar2(" "); TestChar2[0] = component.TestChar2; serializedComponent.SetPrimitiveTypePropertyValue("TestChar2", TestChar2);							serializedComponent.SetPrimitiveTypePropertyValue("TestName", component.TestName);
							std::string TestChar(" "); TestChar[0] = component.TestChar; serializedComponent.SetPrimitiveTypePropertyValue("TestChar", TestChar);							return SerializationResult {};
						},
						.Deserialize = [](SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
						{
							auto& component = entity.AddOrGetComponent<TestComponentC>();
							component.ColorValue = serializedComponent.GetPrimitiveTypePropertyValueOr<Color4>("ColorValue", DYE::Color::Yellow);
							component.TestChar2 = serializedComponent.GetPrimitiveTypePropertyValueOr<const char*>("TestChar2", "2")[0];
							component.TestName = serializedComponent.GetPrimitiveTypePropertyValueOr<String>("TestName", "WHY IS IT LIKE THAT?!");
							component.TestChar = serializedComponent.GetPrimitiveTypePropertyValueOr<const char*>("TestChar", "X")[0];
							return DeserializationResult {};
						},
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							auto& component = entity.GetComponent<TestComponentC>();
							ImGui::TextWrapped("TestComponentC");
							changed |= ImGuiUtil::DrawColor4Control("ColorValue", component.ColorValue);
							changed |= ImGuiUtil::DrawCharControl("TestChar2", component.TestChar2);
							changed |= ImGuiUtil::DrawTextControl("TestName", component.TestName);
							changed |= ImGuiUtil::DrawCharControl("TestChar", component.TestChar);
							return changed;
						}
					}
			);

		// System located in include/SystemExample.h
		static UpdateSystemA _UpdateSystemA;
		TypeRegistry::RegisterSystem("Update System A", &_UpdateSystemA);

		// System located in include/SystemExample.h
		static UpdateSystemB _UpdateSystemB;
		TypeRegistry::RegisterSystem("Update System B", &_UpdateSystemB);

		// System located in include/SystemExample.h
		static FixedUpdateSystem1 _FixedUpdateSystem1;
		TypeRegistry::RegisterSystem("Fixed Update System 1", &_FixedUpdateSystem1);

		// System located in include/SystemExample.h
		static FixedUpdateSystem2 _FixedUpdateSystem2;
		TypeRegistry::RegisterSystem("Fixed Update System 2", &_FixedUpdateSystem2);

		// System located in include/SystemExample.h
		static FixedUpdateSystem3 _FixedUpdateSystem3;
		TypeRegistry::RegisterSystem("Fixed Update System 3", &_FixedUpdateSystem3);

		// System located in include/SystemExample.h
		static SystemNamespace::InitializeSystemA _InitializeSystemA;
		TypeRegistry::RegisterSystem("Initialize System A", &_InitializeSystemA);

		// System located in include/SystemExample.h
		static RotateHasAngularVelocitySystem _RotateHasAngularVelocitySystem;
		TypeRegistry::RegisterSystem("Rotate Has Angular Velocity System", &_RotateHasAngularVelocitySystem);

	}

	class UserTypeRegister_Generated
	{
	public:
		UserTypeRegister_Generated()
		{
			DYE::DYEditor::AssignRegisterUserTypeFunctionPointer(userRegisterTypeFunction);
		}
	};

	[[maybe_unused]]
	static UserTypeRegister_Generated userTypeRegister_GeneratedInstance = UserTypeRegister_Generated();
}
