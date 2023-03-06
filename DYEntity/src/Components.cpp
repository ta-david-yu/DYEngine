#include "Components.h"

#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEntity
{
	void AddNameComponent(Entity& entity)
	{
		entity.AddComponent<NameComponent>("No Name");
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

		ImGui::PushID("##Transform");

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

		ImGui::PopID();

		return changed;
	}

	void RegisterBuiltInComponentTypes()
	{
		TypeRegistry::RegisterComponentType
			(
				"Name",
				ComponentTypeFunctionCollection
					{
						.Has = DefaultHasComponentOfType<NameComponent>,
						.Add = AddNameComponent,
						.Remove = DefaultRemoveComponentOfType<NameComponent>,
						.Serialize = nullptr,
						.Deserialize = nullptr,
						.DrawInspector = DrawInspectorOfNameComponent
					}
			);

		TypeRegistry::RegisterComponentType
			(
				"Transform",
				ComponentTypeFunctionCollection
					{
						.Has = DefaultHasComponentOfType<TransformComponent>,
						.Add = DefaultAddComponentOfType<TransformComponent>,
						.Remove = DefaultRemoveComponentOfType<TransformComponent>,
						.Serialize = nullptr,
						.Deserialize = nullptr,
						.DrawInspector = DrawInspectorOfTransformComponent
					}
			);

		TypeRegistry::RegisterComponentType
			(
				"TestFloat",
				ComponentTypeFunctionCollection
					{
						.Has = DefaultHasComponentOfType<_TestFloatComponent>,
						.Add = DefaultAddComponentOfType<_TestFloatComponent>,
						.Remove = DefaultRemoveComponentOfType<_TestFloatComponent>,
						.Serialize = nullptr,
						.Deserialize = nullptr,
						.DrawInspector = [](Entity& entity)
						{
							ImGui::PushID("##TestFloat");
							bool changed = ImGuiUtil::DrawFloatControl("Value", entity.GetComponent<_TestFloatComponent>().Value, 0.0f);
							ImGui::PopID();
							return changed;
						}
					}
			);

		TypeRegistry::RegisterComponentType
			(
				"TestInt",
				ComponentTypeFunctionCollection
					{
						.Has = DefaultHasComponentOfType<_TestIntComponent>,
						.Add = DefaultAddComponentOfType<_TestIntComponent>,
						.Remove = DefaultRemoveComponentOfType<_TestIntComponent>,
						.Serialize = nullptr,
						.Deserialize = nullptr,
						.DrawInspector = [](Entity& entity)
						{
							ImGui::PushID("##TestInt");
							bool changed = ImGuiUtil::DrawIntControl("Value",
																	 entity.GetComponent<_TestIntComponent>().Value, 0);
							ImGui::PopID();
							return changed;
						}
					}
			);
	}
}