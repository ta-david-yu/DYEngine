#include "BuiltInTypeRegister.h"

#include "TypeRegistry.h"
#include "Serialization/SerializedObjectFactory.h"
#include "ImGui/ImGuiUtil.h"

// All the built-in component types are in here.
#include "Components.h"

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	namespace BuiltInFunctions
	{
		void AddNameComponent(Entity &entity)
		{
			entity.AddComponent<NameComponent>("New Entity");
		}

		SerializationResult SerializeNameComponent(DYE::DYEntity::Entity& entity, SerializedComponentHandle& serializedComponent)
		{
			// We don't do any error handling here (i.e. check if entity has NameComponent) because
			// the function will only be called when the entity has NameComponent for sure.
			serializedComponent.SetPrimitiveTypePropertyValue("Name", entity.GetComponent<NameComponent>().Name);

			return {};
		}

		DeserializationResult DeserializeNameComponent(SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
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

			return {};
		}

		bool DrawInspectorOfNameComponent(Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawTextControl("Name", nameComponent.Name);

			return changed;
		}

		SerializationResult SerializeTransformComponent(DYE::DYEntity::Entity& entity, SerializedComponentHandle& serializedComponent)
		{
			auto const& transformComponent = entity.GetComponent<TransformComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Position", transformComponent.Position);
			serializedComponent.SetPrimitiveTypePropertyValue("Scale", transformComponent.Scale);
			serializedComponent.SetPrimitiveTypePropertyValue("Rotation", transformComponent.Rotation);

			return {};
		}

		DeserializationResult DeserializeTransformComponent(SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity)
		{
			if (entity.HasComponent<NameComponent>())
			{
				auto& transformComponent = entity.GetComponent<TransformComponent>();
				transformComponent.Position = serializedComponent.TryGetPrimitiveTypePropertyValue<DYE::Vector3>("Position").value();
				transformComponent.Scale = serializedComponent.TryGetPrimitiveTypePropertyValue<DYE::Vector3>("Scale").value();
				transformComponent.Rotation = serializedComponent.TryGetPrimitiveTypePropertyValue<DYE::Quaternion>("Rotation").value();
			}
			else
			{
				auto& transformComponent = entity.AddComponent<TransformComponent>();
				transformComponent.Position = serializedComponent.TryGetPrimitiveTypePropertyValue<DYE::Vector3>("Position").value();
				transformComponent.Scale = serializedComponent.TryGetPrimitiveTypePropertyValue<DYE::Vector3>("Scale").value();
				transformComponent.Rotation = serializedComponent.TryGetPrimitiveTypePropertyValue<DYE::Quaternion>("Rotation").value();
			}

			return {};
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

	void RegisterBuiltInTypes()
	{
		DYE_LOG("<< Register Built-in Types to DYEditor::TypeRegistry >>");
		TypeRegistry::RegisterComponentType<NameComponent>
			(
				"Name",
				ComponentTypeFunctionCollection
					{
						.Add = BuiltInFunctions::AddNameComponent,

						.Serialize = BuiltInFunctions::SerializeNameComponent,
						.Deserialize = BuiltInFunctions::DeserializeNameComponent,
						.DrawInspector = BuiltInFunctions::DrawInspectorOfNameComponent,
					}
			);

		TypeRegistry::RegisterComponentType<TransformComponent>
			(
				"Transform",
				ComponentTypeFunctionCollection
					{
						.Serialize = BuiltInFunctions::SerializeTransformComponent,
						.Deserialize = BuiltInFunctions::DeserializeTransformComponent,
						.DrawInspector = BuiltInFunctions::DrawInspectorOfTransformComponent
					}
			);
	}
}