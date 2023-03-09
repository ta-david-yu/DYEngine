#include "UserTypeRegister.h"

#include "TypeRegistry.h"
#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	struct _TestFloatComponent
	{
		float Value;
		float Value2;
		float Value3;
		float Value4;
	};

	struct _TestIntComponent
	{
		int Value;
	};

	struct _TestInt1Component
	{
		int Value;
	};
	struct _TestInt2Component
	{
		int Value;
	};
	struct _TestInt3Component
	{
		int Value;
	};

	void userRegisterTypeFunction()
	{
		DYE_LOG("<< Register User Types to DYEditor::TypeRegistry >>");

		TypeRegistry::RegisterComponentType<_TestFloatComponent>
			(
				"TestFloat",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](DYE::DYEntity::Entity &entity)
						{
							bool changed = false;
							changed |= ImGuiUtil::DrawFloatControl("Value",
																	   entity.GetComponent<_TestFloatComponent>().Value,
																	   0.0f);

							changed |= ImGuiUtil::DrawFloatControl("Value2",
																	   entity.GetComponent<_TestFloatComponent>().Value2,
																	   0.0f);

							changed |= ImGuiUtil::DrawFloatControl("TestFloatValue",
																   entity.GetComponent<_TestFloatComponent>().Value4,
																   0.0f);
							return changed;
						}
					}
			);

		TypeRegistry::RegisterComponentType<_TestIntComponent>
			(
				"TestInt",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = ImGuiUtil::DrawIntControl("Value",
																	 entity.GetComponent<_TestIntComponent>().Value, 0);
							return changed;
						}
					}
			);

		TypeRegistry::RegisterComponentType<_TestInt1Component>
			(
				"TestInt1",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = ImGuiUtil::DrawIntControl("Value",
																	 entity.GetComponent<_TestInt1Component>().Value, 0);
							return changed;
						}
					}
			);

		TypeRegistry::RegisterComponentType<_TestInt2Component>
			(
				"TestInt2",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = ImGuiUtil::DrawIntControl("Value",
																	 entity.GetComponent<_TestInt2Component>().Value, 0);
							return changed;
						}
					}
			);

		TypeRegistry::RegisterComponentType<_TestInt3Component>
			(
				"TestInt3",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = ImGuiUtil::DrawIntControl("Value",
																	 entity.GetComponent<_TestInt3Component>().Value, 0);
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
