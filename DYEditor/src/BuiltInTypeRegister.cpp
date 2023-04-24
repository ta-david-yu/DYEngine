#include "Type/BuiltInTypeRegister.h"

#include "Graphics/Texture.h"
#include "Type/TypeRegistry.h"
#include "Core/EditorProperty.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Math/Color.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/EditorImGuiUtil.h"
#include "FileSystem/FileSystem.h"

// All the built-in component & system types are in here.
#include "Core/Components.h"
#include "Core/Systems.h"
#include "Core/World.h"

#include <filesystem>
#include <cctype>
#include <imgui.h>

namespace DYE::DYEditor
{
	namespace BuiltInFunctions
	{
		SerializationResult
		IDComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			serializedComponent.SetPrimitiveTypePropertyValue("ID", entity.GetComponent<IDComponent>().ID);

			return {};
		}

		DeserializationResult
		IDComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &component = entity.AddOrGetComponent<IDComponent>();
			component.ID = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::GUID>("ID");

			return {};
		}

		bool IDComponent_DrawInspector(Entity &entity)
		{
			auto &component = entity.GetComponent<IDComponent>();

			bool deactivatedAfterEdit = false;
			ImGuiUtil::DrawGUIDControl("ID", component.ID);
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return deactivatedAfterEdit;
		}

		void NameComponent_Add(Entity &entity)
		{
			entity.AddComponent<NameComponent>("New Entity");
		}

		SerializationResult
		NameComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			// We don't do any error handling here (i.e. check if entity has NameComponent) because
			// the function will only be called when the entity has NameComponent for sure.
			serializedComponent.SetPrimitiveTypePropertyValue("Name", entity.GetComponent<NameComponent>().Name);

			return {};
		}

		DeserializationResult
		NameComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &nameComponent = entity.AddOrGetComponent<NameComponent>();
			nameComponent.Name = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::String>("Name");

			return {};
		}

		bool NameComponent_DrawInspector(Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool deactivatedAfterEdit = false;
			ImGuiUtil::DrawTextControl("Name", nameComponent.Name);
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return deactivatedAfterEdit;
		}

		SerializationResult
		TransformComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &transformComponent = entity.GetComponent<TransformComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Position", transformComponent.Position);
			serializedComponent.SetPrimitiveTypePropertyValue("Scale", transformComponent.Scale);
			serializedComponent.SetPrimitiveTypePropertyValue("Rotation", transformComponent.Rotation);

			return {};
		}

		DeserializationResult
		TransformComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &transformComponent = entity.AddOrGetComponent<TransformComponent>();
			transformComponent.Position = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Vector3>(
				"Position");
			transformComponent.Scale = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Vector3>(
				"Scale");
			transformComponent.Rotation = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Quaternion>(
				"Rotation");

			return {};
		}

		bool TransformComponent_DrawInspector(Entity &entity)
		{
			auto &transformComponent = entity.GetComponent<TransformComponent>();

			bool deactivatedAfterEdit = false;

			ImGuiUtil::DrawVector3Control("Position", transformComponent.Position);
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			ImGuiUtil::DrawVector3Control("Scale", transformComponent.Scale, 1.0f);
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(transformComponent.Rotation);
			rotationInEulerAnglesDegree += glm::vec3(0.f);
			rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
			if (ImGuiUtil::DrawVector3Control("Rotation", rotationInEulerAnglesDegree))
			{
				rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
				transformComponent.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};
			}
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return deactivatedAfterEdit;
		}

		void CameraComponent_Add(Entity& entity)
		{
			entity.AddComponent<CameraComponent>();

			// The component only makes sense with a transform component.
			entity.AddOrGetComponent<TransformComponent>();
		}

		SerializationResult
		CameraComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &cameraComponent = entity.GetComponent<CameraComponent>();
			auto const &cameraProperties = cameraComponent.Properties;
			serializedComponent.SetPrimitiveTypePropertyValue("IsEnabled", cameraComponent.IsEnabled);
			serializedComponent.SetPrimitiveTypePropertyValue("ClearColor", cameraProperties.ClearColor);
			serializedComponent.SetPrimitiveTypePropertyValue("Depth", cameraProperties.Depth);

			serializedComponent.SetPrimitiveTypePropertyValue("FiledOfView", cameraProperties.FieldOfView);
			serializedComponent.SetPrimitiveTypePropertyValue("IsOrthographic", cameraProperties.IsOrthographic);
			serializedComponent.SetPrimitiveTypePropertyValue("OrthographicSize", cameraProperties.OrthographicSize);
			serializedComponent.SetPrimitiveTypePropertyValue("NearClipDistance", cameraProperties.NearClipDistance);
			serializedComponent.SetPrimitiveTypePropertyValue("FarClipDistance", cameraProperties.FarClipDistance);

			// TODO: render texture type & render texture reference
			serializedComponent.SetPrimitiveTypePropertyValue("TargetWindowIndex", cameraProperties.TargetWindowIndex);

			serializedComponent.SetPrimitiveTypePropertyValue("UseManualAspectRatio", cameraProperties.UseManualAspectRatio);
			serializedComponent.SetPrimitiveTypePropertyValue("ManualAspectRatio", cameraProperties.ManualAspectRatio);
			serializedComponent.SetPrimitiveTypePropertyValue<DYE::String>("ViewportValueType",
															  cameraProperties.ViewportValueType == ViewportValueType::RelativeDimension?
															  "RelativeDimension" : "AbsoluteDimension");
			serializedComponent.SetPrimitiveTypePropertyValue("Viewport", cameraProperties.Viewport);

			return {};
		}

		DeserializationResult
		CameraComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &cameraComponent = entity.AddOrGetComponent<CameraComponent>();
			auto &cameraProperties = cameraComponent.Properties;
			cameraComponent.IsEnabled = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Bool>("IsEnabled", true);
			cameraProperties.ClearColor = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Color4>("ClearColor");
			cameraProperties.Depth = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Float>("Depth", -1);

			cameraProperties.FieldOfView = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Float>("FiledOfView", 45);
			cameraProperties.IsOrthographic = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Bool>("IsOrthographic");
			cameraProperties.OrthographicSize = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Float>("OrthographicSize", 10);
			cameraProperties.NearClipDistance = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Float>("NearClipDistance", 0.1f);
			cameraProperties.FarClipDistance = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Float>("FarClipDistance", 100);

			// TODO: render texture type & render texture reference
			cameraProperties.TargetWindowIndex = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Int32>("TargetWindowIndex", 0);

			cameraProperties.UseManualAspectRatio = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Bool>("UseManualAspectRatio");
			cameraProperties.ManualAspectRatio = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Float>("ManualAspectRatio");
			auto const& viewportValueTypeAsString = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::String>("ViewportValueType");
			if (viewportValueTypeAsString == "AbsoluteDimension")
			{
				cameraProperties.ViewportValueType = ViewportValueType::AbsoluteDimension;
			}
			else
			{
				cameraProperties.ViewportValueType = ViewportValueType::RelativeDimension;
			}
			cameraProperties.Viewport = serializedComponent.GetPrimitiveTypePropertyValueOr<Math::Rect>("Viewport", {0, 0, 0, 0});

			return {};
		}

		bool CameraComponent_DrawInspector(Entity &entity)
		{
			auto &cameraComponent = entity.GetComponent<CameraComponent>();

			bool deactivatedAfterEdit = false;
			ImGuiUtil::DrawCameraPropertiesControl("Camera Properties", cameraComponent.Properties);
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return deactivatedAfterEdit;
		}

		void SpriteRendererComponent_Add(DYEditor::Entity& entity)
		{
			entity.AddComponent<SpriteRendererComponent>();

			// The component only makes sense with a transform component.
			entity.AddOrGetComponent<TransformComponent>();
		}

		SerializationResult
		SpriteRendererComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &component = entity.GetComponent<SpriteRendererComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Color", component.Color);
			serializedComponent.SetPrimitiveTypePropertyValue("TextureAssetPath", component.TextureAssetPath);

			return {};
		}

		DeserializationResult SpriteRendererComponent_Deserialize(SerializedComponent &serializedComponent,
																  DYE::DYEditor::Entity &entity)
		{
			auto &component = entity.AddOrGetComponent<SpriteRendererComponent>();
			component.Color = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Color4>("Color");
			component.TextureAssetPath = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::AssetPath>(
				"TextureAssetPath");

			if (FileSystem::FileExists(component.TextureAssetPath))
			{
				component.Texture = Texture2D::Create(component.TextureAssetPath);
			}
			else
			{
				component.Texture = Texture2D::GetDefaultTexture();
			}
			
			return {};
		}

		bool SpriteRendererComponent_DrawInspector(Entity &entity)
		{
			auto &component = entity.GetComponent<SpriteRendererComponent>();

			bool deactivatedAfterEdit = false;
			ImGuiUtil::DrawColor4Control("Color", component.Color);
			deactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			bool isPathChanged = ImGuiUtil::DrawAssetPathStringControl("Texture Asset Path", component.TextureAssetPath, {".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd"});
			if (isPathChanged)
			{
				if (FileSystem::FileExists(component.TextureAssetPath))
				{
					component.Texture = Texture2D::Create(component.TextureAssetPath);
				}
				else
				{
					component.Texture = Texture2D::GetDefaultTexture();
				}
			}

			// Draw a preview of the texture
			ImGuiUtil::DrawTexture2DPreviewWithLabel("Texture Preview", component.Texture);

			return isPathChanged || deactivatedAfterEdit;
		}
	}

	static ComponentTypeDescriptor s_NameComponentTypeDescriptor;

	void RegisterBuiltInTypes()
	{
		DYE_LOG("<< Register Built-in Types to DYEditor::TypeRegistry >>");
		TypeRegistry::RegisterComponentType<IDComponent>
		(
			"ID",
			ComponentTypeDescriptor
			{
				.ShouldBeIncludedInNormalAddComponentList = false,
				.ShouldDrawInNormalInspector = false,
				.Serialize = BuiltInFunctions::IDComponent_Serialize,
				.Deserialize = BuiltInFunctions::IDComponent_Deserialize,
				.DrawInspector = BuiltInFunctions::IDComponent_DrawInspector
			}
		);

		s_NameComponentTypeDescriptor = ComponentTypeDescriptor
			{
				.ShouldBeIncludedInNormalAddComponentList = false,
				.ShouldDrawInNormalInspector = false,
				.Add = BuiltInFunctions::NameComponent_Add,

				.Serialize = BuiltInFunctions::NameComponent_Serialize,
				.Deserialize = BuiltInFunctions::NameComponent_Deserialize,
				.DrawInspector = BuiltInFunctions::NameComponent_DrawInspector,
			};
		TypeRegistry::RegisterComponentType<NameComponent>
		(
			NameComponentName,
			s_NameComponentTypeDescriptor
		);

		TypeRegistry::RegisterComponentType<TransformComponent>
		(
			"Transform",
			ComponentTypeDescriptor
			{
				.Serialize = BuiltInFunctions::TransformComponent_Serialize,
				.Deserialize = BuiltInFunctions::TransformComponent_Deserialize,
				.DrawInspector = BuiltInFunctions::TransformComponent_DrawInspector
			}
		);

		TypeRegistry::RegisterComponentType<CameraComponent>
		(
			"Camera",
			ComponentTypeDescriptor
			{
				.Add = BuiltInFunctions::CameraComponent_Add,

				.Serialize = BuiltInFunctions::CameraComponent_Serialize,
				.Deserialize = BuiltInFunctions::CameraComponent_Deserialize,
				.DrawInspector = BuiltInFunctions::CameraComponent_DrawInspector,
				.DrawHeader = DefaultDrawComponentHeaderWithIsEnabled<CameraComponent>
			}
		);

		TypeRegistry::RegisterComponentType<SpriteRendererComponent>
		(
			"Sprite Renderer",
			ComponentTypeDescriptor
				{
					.Add = BuiltInFunctions::SpriteRendererComponent_Add,

					.Serialize = BuiltInFunctions::SpriteRendererComponent_Serialize,
					.Deserialize = BuiltInFunctions::SpriteRendererComponent_Deserialize,
					.DrawInspector = BuiltInFunctions::SpriteRendererComponent_DrawInspector,
					.DrawHeader = DefaultDrawComponentHeaderWithIsEnabled<SpriteRendererComponent>
				}
		);

		static Render2DSpriteSystem _Render2DSpriteSystem;
		TypeRegistry::RegisterSystem(Render2DSpriteSystem::TypeName, &_Render2DSpriteSystem);

		static RegisterCameraSystem _RegisterCameraSystem;
		TypeRegistry::RegisterSystem(RegisterCameraSystem::TypeName, &_RegisterCameraSystem);
	}

	ComponentTypeDescriptor TypeRegistry::GetComponentTypeDescriptor_NameComponent()
	{
		return s_NameComponentTypeDescriptor;
	}
}