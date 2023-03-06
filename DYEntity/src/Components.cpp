#include "Components.h"

#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEntity
{
	void AddNameComponent(Entity& entity)
	{
		entity.AddComponent<NameComponent>("New Entity");
	}

	bool DrawInspectorOfNameComponent(Entity& entity)
	{
		auto& nameComponent = entity.GetComponent<NameComponent>();

		bool changed = false;

		changed |= ImGuiUtil::DrawTextControl("Name", nameComponent.Name);

		return changed;
	}

	bool DrawInspectorOfTransformComponent(Entity& entity)
	{
		auto& transformComponent = entity.GetComponent<TransformComponent>();

		bool changed = false;

		changed |= ImGuiUtil::DrawVec3Control("Position", transformComponent.Position);
		changed |= ImGuiUtil::DrawVec3Control("Scale", transformComponent.Scale);

		glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(transformComponent.Rotation);
		rotationInEulerAnglesDegree += glm::vec3(0.f);
		rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
		if (ImGuiUtil::DrawVec3Control("Rotation", rotationInEulerAnglesDegree))
		{
			rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
			transformComponent.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};

			changed = true;
		}

		return changed;
	}

	struct _TestFloatComponent
	{
		float Value;
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
	void RegisterBuiltInComponentTypes()
	{
		TypeRegistry::RegisterComponentType<NameComponent>
			(
				"Name",
				ComponentTypeFunctionCollection
					{
						.Add = AddNameComponent,
						.DrawInspector = DrawInspectorOfNameComponent
					}
			);

		TypeRegistry::RegisterComponentType<TransformComponent>
			(
				"Transform",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = DrawInspectorOfTransformComponent
					}
			);

		TypeRegistry::RegisterComponentType<_TestFloatComponent>
			(
				"TestFloat",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = ImGuiUtil::DrawFloatControl("Value",
																	   entity.GetComponent<_TestFloatComponent>().Value,
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
}
