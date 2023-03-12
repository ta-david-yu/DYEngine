//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by DYEditorCodeGenerator.
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
#include "UserTypeRegister.h"

#include "TypeRegistry.h"
#include "ImGui/ImGuiUtil.h"

// Insert user headers here...
#include "include/TestComponents.h"
#include "include/AnotherTestComponents.h"

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
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							ImGui::TextWrapped("TestNamespace::TestComponentA");
							ImGui::TextWrapped("FloatValue : Float");
							ImGui::TextWrapped("IntegerValue : Int32");
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
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							ImGui::TextWrapped("TestComponentB");
							ImGui::TextWrapped("BooleanValue : Bool");
							ImGui::TextWrapped("FloatValue : Float");
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
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							ImGui::TextWrapped("TestComponentC");
							ImGui::TextWrapped("ColorValue : Color4");
							return changed;
						}
					}
			);

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
