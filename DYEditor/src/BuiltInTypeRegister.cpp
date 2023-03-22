#include "BuiltInTypeRegister.h"

#include "TypeRegistry.h"
#include "Serialization/SerializedObjectFactory.h"
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

		void SerializeNameComponent(DYE::DYEntity::Entity& entity, SerializedEntity& serializedEntity)
		{
			// We don't do any error handling here (i.e. check if entity has NameComponent) because
			// the function will only be called when the entity has NameComponent for sure.
			SerializedComponentHandle serializedComponent = serializedEntity.TryAddComponentOfType("Name");
			serializedComponent.SetPrimitiveTypePropertyValue("Name", entity.GetComponent<NameComponent>().Name);
		}

		void DeserializeNameComponent(SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
		{
			auto tryGetNameResult = serializedComponent.TryGetPrimitiveTypePropertyValue<std::string>("Name");
			if (!tryGetNameResult.has_value())
			{
				tryGetNameResult.value() = "";
			}

			if (entity.HasComponent<NameComponent>())
			{
				entity.GetComponent<NameComponent>().Name = tryGetNameResult.value();
			}
			else
			{
				entity.AddComponent<NameComponent>(tryGetNameResult.value());
			}
		}

		bool DrawInspectorOfNameComponent(Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawTextControl("Name", nameComponent.Name);

			return changed;
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