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
						.Serialize = nullptr,
						.Deserialize = nullptr,
						.DrawInspector = DrawInspectorOfTransformComponent
					}
			);
	}
}
