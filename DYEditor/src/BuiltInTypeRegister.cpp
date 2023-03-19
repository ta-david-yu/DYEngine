#include "BuiltInTypeRegister.h"

#include "ImGui/ImGuiUtil.h"

#include "TypeRegistry.h"

// All the built-in component types are in here.
#include "Components.h"

using namespace DYE::DYEntity;

namespace DYE::DYEditor
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

		changed |= ImGuiUtil::DrawVector3Control("Position", transformComponent.Position);
		changed |= ImGuiUtil::DrawVector3Control("Scale", transformComponent.Scale);

		glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(transformComponent.Rotation);
		rotationInEulerAnglesDegree += glm::vec3(0.f);
		rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
		if (ImGuiUtil::DrawVector3Control("Rotation", rotationInEulerAnglesDegree))
		{
			rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
			transformComponent.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};

			changed = true;
		}

		return changed;
	}

	void RegisterBuiltInTypes()
	{
		DYE_LOG("<< Register Built-in Types to DYEditor::TypeRegistry >>");
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
	}
}