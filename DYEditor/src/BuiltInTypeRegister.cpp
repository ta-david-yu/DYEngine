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
#include "Components.h"
#include "Systems.h"
#include "World.h"

#include <filesystem>
#include <imgui.h>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	namespace BuiltInFunctions
	{
		void NameComponent_Add(Entity &entity)
		{
			entity.AddComponent<NameComponent>("New Entity");
		}

		SerializationResult
		NameComponent_Serialize(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
		{
			// We don't do any error handling here (i.e. check if entity has NameComponent) because
			// the function will only be called when the entity has NameComponent for sure.
			serializedComponent.SetPrimitiveTypePropertyValue("Name", entity.GetComponent<NameComponent>().Name);

			return {};
		}

		DeserializationResult
		NameComponent_Deserialize(SerializedComponentHandle &serializedComponent, DYE::DYEntity::Entity &entity)
		{
			auto &nameComponent = entity.AddOrGetComponent<NameComponent>();
			nameComponent.Name = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::String>("Name");

			return {};
		}

		bool NameComponent_DrawInspector(Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawTextControl("Name", nameComponent.Name);

			return changed;
		}

		SerializationResult
		TransformComponent_Serialize(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
		{
			auto const &transformComponent = entity.GetComponent<TransformComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Position", transformComponent.Position);
			serializedComponent.SetPrimitiveTypePropertyValue("Scale", transformComponent.Scale);
			serializedComponent.SetPrimitiveTypePropertyValue("Rotation", transformComponent.Rotation);

			return {};
		}

		DeserializationResult
		TransformComponent_Deserialize(SerializedComponentHandle &serializedComponent, DYE::DYEntity::Entity &entity)
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

			bool changed = false;

			changed |= ImGuiUtil::DrawVector3Control("Position", transformComponent.Position);
			changed |= ImGuiUtil::DrawVector3Control("Scale", transformComponent.Scale, 1.0f);

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

		void CameraComponent_Add(Entity& entity)
		{
			entity.AddComponent<CameraComponent>();

			// The component only makes sense with a transform component.
			entity.AddOrGetComponent<TransformComponent>();
		}

		SerializationResult
		CameraComponent_Serialize(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
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

			// TODO: target type & render target ID
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
		CameraComponent_Deserialize(SerializedComponentHandle &serializedComponent, DYE::DYEntity::Entity &entity)
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

			// TODO: target type & render target ID
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

			bool changed = false;
			changed |= ImGuiUtil::DrawCameraPropertiesControl("Camera Properties", cameraComponent.Properties);
			return changed;
		}

		void SpriteRendererComponent_Add(DYEntity::Entity& entity)
		{
			entity.AddComponent<SpriteRendererComponent>();

			// The component only makes sense with a transform component.
			entity.AddOrGetComponent<TransformComponent>();
		}

		SerializationResult
		SpriteRendererComponent_Serialize(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
		{
			auto const &component = entity.GetComponent<SpriteRendererComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Color", component.Color);
			serializedComponent.SetPrimitiveTypePropertyValue("TextureAssetPath", component.TextureAssetPath);

			return {};
		}

		DeserializationResult SpriteRendererComponent_Deserialize(SerializedComponentHandle &serializedComponent,
																  DYE::DYEntity::Entity &entity)
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

			bool changed = false;

			changed |= ImGuiUtil::DrawColor4Control("Color", component.Color);

			// TODO: add a asset path imgui control
			//  For now, we use a simple string editor,
			// 	We will eventually use ImGuiUtil::DrawAssetPathStringControl("Texture Path", component.TextureAssetPath);
			auto pathAsString = component.TextureAssetPath.string();
			bool isPathChanged = ImGuiUtil::DrawTextControl("Texture Asset Path", pathAsString);
			if (isPathChanged)
			{
				component.TextureAssetPath = pathAsString;
			}
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 const buttonSize = {lineHeight + 3.0f, lineHeight};

			// Draw path selection popup button.
			char const* popupId = "Select a file (*.jpg, *.jpeg, *.png, *.tga, *.bmp, *.psd)";
			ImGui::SameLine();
			if (ImGui::Button("S", buttonSize))
			{
				ImGuiUtil::OpenFilePathPopup(
					popupId,
					"assets",
					component.TextureAssetPath,
					{".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd"});
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::TextUnformatted("Open a path selection window");
				ImGui::EndTooltip();
			}

			ImGuiUtil::FilePathPopupResult result = ImGuiUtil::DrawFilePathPopup(popupId, component.TextureAssetPath);
			if (result == ImGuiUtil::FilePathPopupResult::Confirm)
			{
				isPathChanged = true;
			}

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

			changed |= isPathChanged;

			// Draw a preview of the texture
			ImGuiUtil::DrawTexture2DPreviewWithLabel("Texture Preview", component.Texture);

			return changed;
		}
	}

	void RegisterBuiltInTypes()
	{
		DYE_LOG("<< Register Built-in Types to DYEditor::TypeRegistry >>");
		TypeRegistry::RegisterComponentType<NameComponent>
			(
				NameComponentName,
				ComponentTypeFunctionCollection
					{
						.Add = BuiltInFunctions::NameComponent_Add,

						.Serialize = BuiltInFunctions::NameComponent_Serialize,
						.Deserialize = BuiltInFunctions::NameComponent_Deserialize,
						.DrawInspector = BuiltInFunctions::NameComponent_DrawInspector,
					}
			);

		TypeRegistry::RegisterComponentType<TransformComponent>
			(
				"Transform",
				ComponentTypeFunctionCollection
					{
						.Serialize = BuiltInFunctions::TransformComponent_Serialize,
						.Deserialize = BuiltInFunctions::TransformComponent_Deserialize,
						.DrawInspector = BuiltInFunctions::TransformComponent_DrawInspector
					}
			);

		TypeRegistry::RegisterComponentType<CameraComponent>
		    (
				"Camera",
				ComponentTypeFunctionCollection
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
				ComponentTypeFunctionCollection
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
}