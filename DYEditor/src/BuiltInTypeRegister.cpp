#include "BuiltInTypeRegister.h"

#include "TypeRegistry.h"
#include "Serialization/SerializedObject.h"
#include "ImGui/ImGuiUtil.h"

// All the built-in component types are in here.
#include "Components.h"

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	void RegisterBuiltInTypes()
	{
		DYE_LOG("<< Register Built-in Types to DYEditor::TypeRegistry >>");
		TypeRegistry::RegisterComponentType<NameComponent>
			(
				"Name",
				ComponentTypeFunctionCollection
					{
						.Add = AddNameComponent,

						.Serialize = SerializeNameComponent,
						.Deserialize = DeserializeNameComponent,
						.DrawInspector = DrawInspectorOfNameComponent,
					}
			);

		TypeRegistry::RegisterComponentType<TransformComponent>
			(
				"Transform",
				ComponentTypeFunctionCollection
					{
						.Serialize = nullptr,
						.Deserialize = nullptr,
						.DrawInspector = DrawInspectorOfTransformComponent
					}
			);
	}

	namespace
	{
		void AddNameComponent(Entity &entity)
		{
			entity.AddComponent<NameComponent>("New Entity");
		}

		bool DrawInspectorOfNameComponent(Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawTextControl("Name", nameComponent.Name);

			return changed;
		}

		void SerializeNameComponent(DYE::DYEntity::Entity& entity, SerializedEntity& serializedEntity)
		{
			// TODO: get Name value from serializedComponent and add/change entity.GetComponent<NameComponent>().Name
			//		for now we assume the serializedEntity is empty
			SerializedComponentHandle serializedComponent = serializedEntity.TryAddComponentOfType("Name");

		}

		void DeserializeNameComponent(SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
		{
			// TODO: get Name value from serializedComponent and add/change entity.GetComponent<NameComponent>().Name
		}

		bool DrawInspectorOfTransformComponent(Entity &entity)
		{
			auto &transformComponent = entity.GetComponent<TransformComponent>();

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
	}
}