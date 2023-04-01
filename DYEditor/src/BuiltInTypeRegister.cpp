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

#include <filesystem>
#include <imgui.h>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	namespace BuiltInFunctions
	{
		void AddNameComponent(Entity &entity)
		{
			entity.AddComponent<NameComponent>("New Entity");
		}

		SerializationResult
		SerializeNameComponent(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
		{
			// We don't do any error handling here (i.e. check if entity has NameComponent) because
			// the function will only be called when the entity has NameComponent for sure.
			serializedComponent.SetPrimitiveTypePropertyValue("Name", entity.GetComponent<NameComponent>().Name);

			return {};
		}

		DeserializationResult
		DeserializeNameComponent(SerializedComponentHandle &serializedComponent, DYE::DYEntity::Entity &entity)
		{
			auto &nameComponent = entity.AddOrGetComponent<NameComponent>();
			nameComponent.Name = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::String>("Name");

			return {};
		}

		bool DrawInspectorOfNameComponent(Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawTextControl("Name", nameComponent.Name);

			return changed;
		}

		SerializationResult
		SerializeTransformComponent(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
		{
			auto const &transformComponent = entity.GetComponent<TransformComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Position", transformComponent.Position);
			serializedComponent.SetPrimitiveTypePropertyValue("Scale", transformComponent.Scale);
			serializedComponent.SetPrimitiveTypePropertyValue("Rotation", transformComponent.Rotation);

			return {};
		}

		DeserializationResult
		DeserializeTransformComponent(SerializedComponentHandle &serializedComponent, DYE::DYEntity::Entity &entity)
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

		SerializationResult
		SerializeSpriteRendererComponent(DYE::DYEntity::Entity &entity, SerializedComponentHandle &serializedComponent)
		{
			auto const &component = entity.GetComponent<SpriteRendererComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Color", component.Color);
			serializedComponent.SetPrimitiveTypePropertyValue("TextureAssetPath", component.TextureAssetPath);

			return {};
		}

		DeserializationResult DeserializeSpriteRendererComponent(SerializedComponentHandle &serializedComponent,
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

		bool DrawInspectorOfSpriteRendererComponent(Entity &entity)
		{
			auto &component = entity.GetComponent<SpriteRendererComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawBoolControl("Is Enabled", component.IsEnabled);
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
			ImGui::SameLine();
			if (ImGui::Button("S", buttonSize))
			{
				ImGuiUtil::OpenFilePathPopup("assets", component.TextureAssetPath);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::TextUnformatted("Open a path selection window");
				ImGui::EndTooltip();
			}

			ImGuiUtil::FilePathPopupResult result = ImGuiUtil::DrawFilePathPopup(component.TextureAssetPath);
			if (result == ImGuiUtil::FilePathPopupResult::Save)
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

		TypeRegistry::RegisterComponentType<SpriteRendererComponent>
		    (
				"SpriteRenderer",
				ComponentTypeFunctionCollection
					{
						.Serialize = BuiltInFunctions::SerializeSpriteRendererComponent,
						.Deserialize = BuiltInFunctions::DeserializeSpriteRendererComponent,
						.DrawInspector = BuiltInFunctions::DrawInspectorOfSpriteRendererComponent
					}
			);

		static Render2DSpriteSystem _Render2DSpriteSystem;
		TypeRegistry::RegisterSystem("Render 2D Sprite System", &_Render2DSpriteSystem);
	}
}