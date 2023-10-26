#include "Type/BuiltInTypeRegister.h"

#include "Graphics/Texture.h"
#include "Type/TypeRegistry.h"
#include "Core/EditorProperty.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Math/Color.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/EditorImGuiUtil.h"
#include "ImGui/ImGuiUtil_Internal.h"
#include "FileSystem/FileSystem.h"
#include "Core/RuntimeState.h"
#include "Undo/Undo.h"

// All the built-in component & system types are in here.
#include "Core/Components.h"
#include "Core/Systems.h"
#include "Core/World.h"

#include <filesystem>
#include <cctype>
#include <imgui.h>

namespace DYE::DYEditor
{
	namespace BuiltInComponentTypeFunctions
	{
		template<typename T>
		SerializationResult
		SerializeEmptyComponent(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			return {};
		}

		template<typename T>
		DeserializationResult
		DeserializeEmptyComponent(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			entity.AddComponent<T>();
			return {};
		}

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

		bool IDComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &component = entity.GetComponent<IDComponent>();

			bool const changed = ImGuiUtil::DrawGUIDControl("ID", component.ID);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return changed;
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

		bool NameComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &nameComponent = entity.GetComponent<NameComponent>();

			bool const changed = ImGuiUtil::DrawTextControl("Name", nameComponent.Name);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return changed;
		}

		SerializationResult
		ParentComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			serializedComponent.SetPrimitiveTypePropertyValue("ParentGUID", entity.GetComponent<ParentComponent>().GetParentGUID());

			return {};
		}

		DeserializationResult
		ParentComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &parentComponent = entity.AddOrGetComponent<ParentComponent>();

			GUID deserializedParentGUID = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::GUID>("ParentGUID");
			parentComponent.SetParentGUID(deserializedParentGUID);

			return {};
		}

		bool ParentComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &parentComponent = entity.GetComponent<ParentComponent>();

			GUID parentGUID = parentComponent.GetParentGUID();
			bool const changed = ImGuiUtil::DrawGUIDControl("ParentGUID", parentGUID);
			if (changed)
			{
				parentComponent.TrySetParentGUIDIfFoundInWorld(parentGUID, entity.GetWorld());
			}

			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return changed;
		}

		SerializationResult
		ChildrenComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &childrenGUIDs = entity.GetComponent<ChildrenComponent>().GetChildrenGUIDs();
			SerializedArray serializedArray;

			for (int i = 0; i < childrenGUIDs.size(); ++i)
			{
				DYE::GUID const &childGUID = childrenGUIDs[i];
				serializedArray.InsertElementAtIndex(i, childGUID);
			}

			serializedComponent.SetArrayPropertyValue("Children", std::move(serializedArray));
			return {};
		}

		DeserializationResult
		ChildrenComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &childrenComponent = entity.AddOrGetComponent<ChildrenComponent>();
			childrenComponent.m_ChildrenGUIDs.clear();
			childrenComponent.m_ChildrenEntityIdentifiersCache.clear();

			auto tryGetSerializedArray = serializedComponent.TryGetArrayProperty("Children");
			if (!tryGetSerializedArray.has_value())
			{
				return {};
			}

			auto &serializedArray = tryGetSerializedArray.value();
			childrenComponent.m_ChildrenGUIDs.reserve(serializedArray.Size());
			for (int i = 0; i < serializedArray.Size(); i++)
			{
				auto tryGetElement = serializedArray.TryGetElementAtIndex<GUID>(i);
				if (!tryGetElement.has_value())
				{
					continue;
				}
				childrenComponent.m_ChildrenGUIDs.push_back(tryGetElement.value());
			}

			childrenComponent.RefreshChildrenEntityIdentifierCache(entity.GetWorld());

			return {};
		}

		bool ChildrenComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &childrenComponent = entity.GetComponent<ChildrenComponent>();

			bool changed = false;
			auto &childrenGUIDs = childrenComponent.m_ChildrenGUIDs;

			ImGuiUtil::Internal::GUIDControlFunctionType* lambda = [](std::vector<::DYE::GUID> &array, std::size_t index) -> bool
			{
				char elementControlID[16]; sprintf(elementControlID, "Element %zu", index);
				bool const isGUIDChanged = ImGuiUtil::DrawGUIDControl(std::string(elementControlID), array[index]);
				return isGUIDChanged;
			};
			bool const isGUIDsArrayChanged = ImGuiUtil::Internal::ArrayControl("Children", childrenGUIDs, lambda).Draw();

			if (isGUIDsArrayChanged)
			{
				// TODO: We want to improve this in the future, we only want to update the cache index, not the whole vector array.
				childrenComponent.RefreshChildrenEntityIdentifierCache(entity.GetWorld());
			}

			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return changed || isGUIDsArrayChanged;
		}

		SerializationResult
		LocalTransformComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &transformComponent = entity.GetComponent<LocalTransformComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("Position", transformComponent.Position);
			serializedComponent.SetPrimitiveTypePropertyValue("Scale", transformComponent.Scale);
			serializedComponent.SetPrimitiveTypePropertyValue("Rotation", transformComponent.Rotation);

			return {};
		}

		DeserializationResult
		LocalTransformComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &transformComponent = entity.AddOrGetComponent<LocalTransformComponent>();
			transformComponent.Position = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Vector3>("Position");
			transformComponent.Scale = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Vector3>("Scale");
			transformComponent.Rotation = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Quaternion>("Rotation");

			return {};
		}

		bool LocalTransformComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &transformComponent = entity.GetComponent<LocalTransformComponent>();

			bool changed = ImGuiUtil::DrawVector3Control("Position", transformComponent.Position);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			changed |= ImGuiUtil::DrawVector3Control("Scale", transformComponent.Scale, 1.0f);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(transformComponent.Rotation);
			rotationInEulerAnglesDegree += glm::vec3(0.f);
			rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
			if (ImGuiUtil::DrawVector3Control("Rotation", rotationInEulerAnglesDegree))
			{
				rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
				transformComponent.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};
				changed = true;
			}
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			return changed;
		}

		SerializationResult
		LocalToWorldComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			return {};
		}

		DeserializationResult
		LocalToWorldComponent_Deserialize(SerializedComponent &serializedComponent, DYE::DYEditor::Entity &entity)
		{
			auto &localToWorldComponent = entity.AddOrGetComponent<LocalToWorldComponent>();
			return {};
		}

		bool LocalToWorldComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			bool changed = false;
			auto &localToWorldComponent = entity.GetComponent<LocalToWorldComponent>();

			ImGui::BeginDisabled();

			auto position = localToWorldComponent.GetPosition();
			ImGuiUtil::DrawVector3Control("World Position", position);

			glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(localToWorldComponent.GetRotation());
			rotationInEulerAnglesDegree += glm::vec3(0.f);
			rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
			ImGuiUtil::DrawVector3Control("World Rotation", rotationInEulerAnglesDegree);

			ImGui::EndDisabled();

			bool isMissingComponentWarningFixed = ImGuiUtil::DrawTryFixWarningButtonAndInfo
				(
					!entity.HasComponent<LocalTransformComponent>(),
					"Missing LocalTransform component",
					[entity]()
					{
						Undo::AddComponent(entity, LocalTransformComponentTypeName,
										   TypeRegistry::GetComponentTypeDescriptor_LocalTransformComponent());
					}
				);
			if (isMissingComponentWarningFixed)
			{
				changed = true;
				drawInspectorContext.ShouldEarlyOutIfInIteratorLoop = true;
			}

			return changed;
		}

		void CameraComponent_Add(Entity& entity)
		{
			entity.AddComponent<CameraComponent>();
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

		bool CameraComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &cameraComponent = entity.GetComponent<CameraComponent>();

			bool changed = ImGuiUtil::DrawCameraPropertiesControl("Camera Properties", cameraComponent.Properties);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			bool isMissingComponentWarningFixed = ImGuiUtil::DrawTryFixWarningButtonAndInfo
				(
					!entity.HasComponent<LocalToWorldComponent>(),
					"Missing LocalToWorld component",
					[entity]()
					{
						Undo::AddComponent(entity, LocalToWorldComponentTypeName,
										   TypeRegistry::GetComponentTypeDescriptor_LocalToWorldComponent());
					}
				);
			if (isMissingComponentWarningFixed)
			{
				changed = true;
				drawInspectorContext.ShouldEarlyOutIfInIteratorLoop = true;
			}

			return changed;
		}

		void SpriteRendererComponent_Add(DYEditor::Entity& entity)
		{
			entity.AddComponent<SpriteRendererComponent>();
		}

		SerializationResult
		SpriteRendererComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &component = entity.GetComponent<SpriteRendererComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("IsEnabled", component.IsEnabled);
			serializedComponent.SetPrimitiveTypePropertyValue("Color", component.Color);
			serializedComponent.SetPrimitiveTypePropertyValue("TextureAssetPath", component.TextureAssetPath);

			return {};
		}

		DeserializationResult SpriteRendererComponent_Deserialize(SerializedComponent &serializedComponent,
																  DYE::DYEditor::Entity &entity)
		{
			auto &component = entity.AddOrGetComponent<SpriteRendererComponent>();
			component.IsEnabled = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Bool>("IsEnabled", true);
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

		bool SpriteRendererComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &component = entity.GetComponent<SpriteRendererComponent>();

			bool changed = ImGuiUtil::DrawColor4Control("Color", component.Color);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			bool isPathChanged = ImGuiUtil::DrawAssetPathStringControl("Texture Asset Path", component.TextureAssetPath, {".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd"});
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

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

			// Draw a preview of the texture.
			ImGuiUtil::DrawTexture2DPreviewWithLabel("Texture Preview", component.Texture);

			bool isMissingComponentWarningFixed = ImGuiUtil::DrawTryFixWarningButtonAndInfo
				(
					!entity.HasComponent<LocalToWorldComponent>(),
					"Missing LocalToWorld component",
					[entity]()
					{
						Undo::AddComponent(entity, LocalToWorldComponentTypeName,
										   TypeRegistry::GetComponentTypeDescriptor_LocalToWorldComponent());
					}
				);
			if (isMissingComponentWarningFixed)
			{
				changed = true;
				drawInspectorContext.ShouldEarlyOutIfInIteratorLoop = true;
			}

			return isPathChanged || changed;
		}

		SerializationResult AudioSource2DComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &component = entity.GetComponent<AudioSource2DComponent>();

			serializedComponent.SetPrimitiveTypePropertyValue("Volume", component.Source.GetVolume());
			std::string loadTypeString;
			switch (component.LoadType)
			{
				case AudioLoadType::DecompressOnLoad:
					loadTypeString = "DecompressOnLoad";
					break;
				case AudioLoadType::Streaming:
					loadTypeString = "Streaming";
					break;
			}

			serializedComponent.SetPrimitiveTypePropertyValue<DYE::String>("LoadType", loadTypeString);
			serializedComponent.SetPrimitiveTypePropertyValue<DYE::Bool>("IsStreamLooping", component.Source.IsStreamLooping());
			serializedComponent.SetPrimitiveTypePropertyValue("ClipAssetPath", component.ClipAssetPath);

			return {};
		}

		DeserializationResult AudioSource2DComponent_Deserialize(SerializedComponent &serializedComponent,
																  DYE::DYEditor::Entity &entity)
		{
			auto &component = entity.AddOrGetComponent<AudioSource2DComponent>();

			component.Source.SetVolume(serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Float>("Volume", 0));

			auto const& loadTypeString = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::String>("LoadType", "DecompressOnLoad");
			if (loadTypeString == "DecompressOnLoad")
			{
				component.LoadType = AudioLoadType::DecompressOnLoad;
			}
			else if (loadTypeString == "Streaming")
			{
				component.LoadType = AudioLoadType::Streaming;
			}
			else
			{
				component.LoadType = AudioLoadType::DecompressOnLoad;
			}

			bool const isLooping = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::Bool>("IsStreamLooping");
			component.Source.SetStreamLooping(isLooping);

			component.ClipAssetPath = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<DYE::AssetPath>("ClipAssetPath");

			auto path = component.ClipAssetPath;
			if (FileSystem::FileExists(path))
			{
				component.Source.SetClip(AudioClip::Create(path, { .LoadType = component.LoadType }));
			}

			return {};
		}

		bool AudioSource2DComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &component = entity.GetComponent<AudioSource2DComponent>();

			bool changed = false;

			float volume = component.Source.GetVolume();
			if (ImGuiUtil::DrawFloatSliderControl("Volume", volume, 0, 1))
			{
				component.Source.SetVolume(volume);
				changed = true;
			}
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			auto loadTypeIndex = (std::int32_t) component.LoadType;
			bool const loadTypeChanged = ImGuiUtil::DrawDropdownControl("Load Type", loadTypeIndex, { "DecompressOnLoad", "Streaming" });
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
			if (loadTypeChanged)
			{
				component.LoadType = (AudioLoadType) loadTypeIndex;
			}

			if (component.LoadType == AudioLoadType::Streaming)
			{
				bool isLooping = component.Source.IsStreamLooping();
				if (ImGuiUtil::DrawBoolControl("Is Stream Looping", isLooping))
				{
					component.Source.SetStreamLooping(isLooping);
				}
				drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
				drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
				drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
			}

			bool const isPathChanged = ImGuiUtil::DrawAssetPathStringControl("Clip Asset Path", component.ClipAssetPath, {".wav", ".mp3", ".ogg", ".mod", ".flac"});
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			bool const needReloadClip = loadTypeChanged || isPathChanged;
			if (needReloadClip)
			{
				if (FileSystem::FileExists(component.ClipAssetPath))
				{
					component.Source.SetClip(AudioClip::Create(component.ClipAssetPath, { .LoadType = component.LoadType }));
				}
				else
				{
					component.Source.SetClip(nullptr);
				}
			}

			// TODO: Draw a audio player.
			// 	like that for texture: ImGuiUtil::DrawTexture2DPreviewWithLabel("Texture Preview", component.Texture);

			// In Play Mode we want to draw a helper audio player.
			if (RuntimeState::IsPlaying())
			{
				ImGui::Separator();
				if (ImGui::TreeNodeEx("Play Mode Audio Source Debugger", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Separator();
					if (ImGui::Button("Play"))
					{
						entity.AddComponent<StartAudioSourceComponent>();
					}
					ImGui::SameLine();
					if (ImGui::Button("Stop"))
					{
						entity.AddComponent<StopAudioSourceComponent>();
					}

					ImGui::TreePop();
				}
			}

			return loadTypeChanged || isPathChanged || changed;
		}

		bool PlayAudioSourceOnInitializeComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			ImGui::Indent();
			ImGui::TextWrapped("The AudioSource attached to this entity will be played the first frame after the scene is loaded.");
			ImGui::Unindent();
			return false;
		}

		bool WindowHandleComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &component = entity.GetComponent<WindowHandleComponent>();

			ImGui::BeginDisabled(true);

			ImGuiUtil::DrawBoolControl("Is Open", component.IsCreated);
			if (!component.IsCreated)
			{
				ImGui::EndDisabled();
				return false;
			}

			WindowBase* pWindow = component.TryGetWindow();
			DYE_ASSERT(pWindow != nullptr);

			ImGui::Separator();
			ImGuiUtil::DrawReadOnlyTextWithLabel("ID", std::to_string(component.ID));
			ImGuiUtil::DrawReadOnlyTextWithLabel("Index", std::to_string(component.Index));
			ImGuiUtil::DrawReadOnlyTextWithLabel("Title", pWindow->GetTitle());

			switch (pWindow->GetFullScreenMode())
			{
				case FullScreenMode::Window:
					ImGuiUtil::DrawReadOnlyTextWithLabel("Mode", "Window");
					break;
				case FullScreenMode::FullScreen:
					ImGuiUtil::DrawReadOnlyTextWithLabel("Mode", "FullScreen");
					break;
				case FullScreenMode::FullScreenWithDesktopResolution:
					ImGuiUtil::DrawReadOnlyTextWithLabel("Mode", "FullScreenWithDesktopResolution");
					break;
			}

			auto const position = pWindow->GetPosition();
			ImGuiUtil::DrawReadOnlyTextWithLabel("Position", "(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
			auto const size = pWindow->GetSize();
			ImGuiUtil::DrawReadOnlyTextWithLabel("Size", "(" + std::to_string(size.x) + ", " + std::to_string(size.y) + ")");

			ImGui::EndDisabled();

			// If the window handle assigned with a window, we want to draw a helper window debugger.
			if (pWindow != nullptr)
			{
				ImGui::Separator();
				if (ImGui::TreeNodeEx("Window Debugger", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Separator();
					if (ImGui::Button("Minimize"))
					{
						pWindow->Minimize();
					}
					ImGui::SameLine();
					if (ImGui::Button("Restore"))
					{
						pWindow->Restore();
					}
					ImGui::SameLine();
					if (ImGui::Button("Raise"))
					{
						pWindow->Raise();
					}
					ImGui::SameLine();
					if (ImGui::Button("Center"))
					{
						pWindow->CenterWindow();
					}

					ImGui::TreePop();
				}
			}

			return false;
		}

		SerializationResult
		CreateWindowOnInitializeComponent_Serialize(DYE::DYEditor::Entity &entity, SerializedComponent &serializedComponent)
		{
			auto const &component = entity.GetComponent<CreateWindowOnInitializeComponent>();
			serializedComponent.SetPrimitiveTypePropertyValue("HasInitialPosition", component.HasInitialPosition);
			serializedComponent.SetPrimitiveTypePropertyValue("InitialPosition", component.InitialPosition);
			serializedComponent.SetPrimitiveTypePropertyValue("InitialWidth", component.InitialWidth);
			serializedComponent.SetPrimitiveTypePropertyValue("InitialHeight", component.InitialHeight);
			serializedComponent.SetPrimitiveTypePropertyValue("Title", component.Title);
			return {};
		}

		DeserializationResult CreateWindowOnInitializeComponent_Deserialize(SerializedComponent &serializedComponent,
																  DYE::DYEditor::Entity &entity)
		{
			auto &component = entity.AddOrGetComponent<CreateWindowOnInitializeComponent>();
			component.HasInitialPosition = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Bool>("HasInitialPosition", false);
			component.InitialPosition = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Vector2>("InitialPosition", {0, 0});
			component.InitialWidth = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Int32>("InitialWidth", 1600);
			component.InitialHeight = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::Int32>("InitialHeight", 900);
			component.Title = serializedComponent.GetPrimitiveTypePropertyValueOr<DYE::String>("Title", "New Window");
			return {};
		}

		bool CreateWindowOnInitializeComponent_DrawInspector(DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
		{
			auto &component = entity.GetComponent<CreateWindowOnInitializeComponent>();

			bool changed = false;

			changed |= ImGuiUtil::DrawTextControl("Title", component.Title);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			changed |= ImGuiUtil::DrawIntSliderControl("Initial Width", component.InitialWidth, 1, 7680);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			changed |= ImGuiUtil::DrawIntSliderControl("Initial Height", component.InitialHeight, 1, 4320);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			changed |= ImGuiUtil::DrawBoolControl("Has Initial Position", component.HasInitialPosition);
			drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
			drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
			drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

			if (component.HasInitialPosition)
			{
				changed |= ImGuiUtil::DrawVector2Control("Initial Position", component.InitialPosition);
				drawInspectorContext.IsModificationActivated |= ImGuiUtil::IsControlActivated();
				drawInspectorContext.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
				drawInspectorContext.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
			}

			return changed;
		}
	}

	static ComponentTypeDescriptor s_NameComponentTypeDescriptor;
	static ComponentTypeDescriptor s_ParentComponentTypeDescriptor;
	static ComponentTypeDescriptor s_ChildrenComponentTypeDescriptor;
	static ComponentTypeDescriptor s_LocalTransformComponentTypeDescriptor;
	static ComponentTypeDescriptor s_LocalToWorldComponentTypeDescriptor;

	void RegisterBuiltInTypes()
	{
		DYE_LOG("<< Register Built-in Types to DYEditor::TypeRegistry >>");
		TypeRegistry::RegisterComponentType<IDComponent>
		(
			NAME_OF(DYE::DYEditor::IDComponent),
			ComponentTypeDescriptor
			{
				.ShouldBeIncludedInNormalAddComponentList = false,
				.ShouldDrawInNormalInspector = false,
				.Serialize = BuiltInComponentTypeFunctions::IDComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::IDComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::IDComponent_DrawInspector,
				.GetDisplayName = []() { return "ID"; },
			}
		);
		TypeRegistry::RegisterFormerlyKnownTypeName("ID", NAME_OF(DYE::DYEditor::IDComponent));

		s_NameComponentTypeDescriptor = TypeRegistry::RegisterComponentType<NameComponent>
		(
			NameComponentTypeName,
			ComponentTypeDescriptor
			{
				.ShouldBeIncludedInNormalAddComponentList = false,
				.ShouldDrawInNormalInspector = false,
				.Add = BuiltInComponentTypeFunctions::NameComponent_Add,

				.Serialize = BuiltInComponentTypeFunctions::NameComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::NameComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::NameComponent_DrawInspector,
				.GetDisplayName = []() { return "Name"; },
			}
		);
		TypeRegistry::RegisterFormerlyKnownTypeName("Name", NameComponentTypeName);

		s_ParentComponentTypeDescriptor = TypeRegistry::RegisterComponentType<ParentComponent>
		(
			ParentComponentTypeName,
			ComponentTypeDescriptor
			{
				.ShouldBeIncludedInNormalAddComponentList = false,
				.ShouldDrawInNormalInspector = false,
				.Serialize = BuiltInComponentTypeFunctions::ParentComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::ParentComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::ParentComponent_DrawInspector,
				.GetDisplayName = []() { return "Parent"; },
			}
		);
		TypeRegistry::RegisterFormerlyKnownTypeName("Parent", ParentComponentTypeName);


		s_ChildrenComponentTypeDescriptor = TypeRegistry::RegisterComponentType<ChildrenComponent>
		(
			ChildrenComponentTypeName,
			ComponentTypeDescriptor
			{
				.ShouldBeIncludedInNormalAddComponentList = false,
				.ShouldDrawInNormalInspector = false,
				.Serialize = BuiltInComponentTypeFunctions::ChildrenComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::ChildrenComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::ChildrenComponent_DrawInspector,
				.GetDisplayName = []() { return "Children"; },
			}
		);
		TypeRegistry::RegisterFormerlyKnownTypeName("Children", ChildrenComponentTypeName);

		s_LocalTransformComponentTypeDescriptor = TypeRegistry::RegisterComponentType<LocalTransformComponent>
		(
			LocalTransformComponentTypeName,
			ComponentTypeDescriptor
				{
					.Serialize = BuiltInComponentTypeFunctions::LocalTransformComponent_Serialize,
					.Deserialize = BuiltInComponentTypeFunctions::LocalTransformComponent_Deserialize,
					.DrawInspector = BuiltInComponentTypeFunctions::LocalTransformComponent_DrawInspector,
					.GetDisplayName = []() { return "Local Transform"; },
				}
		);

		s_LocalToWorldComponentTypeDescriptor = TypeRegistry::RegisterComponentType<LocalToWorldComponent>
		(
			LocalToWorldComponentTypeName,
			ComponentTypeDescriptor
				{
					.Serialize = BuiltInComponentTypeFunctions::LocalToWorldComponent_Serialize,
					.Deserialize = BuiltInComponentTypeFunctions::LocalToWorldComponent_Deserialize,
					.DrawInspector = BuiltInComponentTypeFunctions::LocalToWorldComponent_DrawInspector,
					.GetDisplayName = []() { return "Local To World"; },
				}
		);

		TypeRegistry::RegisterComponentType<CameraComponent>
		(
			NAME_OF(DYE::DYEditor::CameraComponent),
			ComponentTypeDescriptor
			{
				.Add = BuiltInComponentTypeFunctions::CameraComponent_Add,

				.Serialize = BuiltInComponentTypeFunctions::CameraComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::CameraComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::CameraComponent_DrawInspector,
				.DrawHeader = DefaultDrawComponentHeaderWithIsEnabled<CameraComponent>,
				.GetDisplayName = []() { return "Camera"; },
			}
		);

		TypeRegistry::RegisterComponentType<SpriteRendererComponent>
		(
			NAME_OF(DYE::DYEditor::SpriteRendererComponent),
			ComponentTypeDescriptor
			{
				.Add = BuiltInComponentTypeFunctions::SpriteRendererComponent_Add,

				.Serialize = BuiltInComponentTypeFunctions::SpriteRendererComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::SpriteRendererComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::SpriteRendererComponent_DrawInspector,
				.DrawHeader = DefaultDrawComponentHeaderWithIsEnabled<SpriteRendererComponent>,
				.GetDisplayName = []() { return "Sprite Renderer"; },
			}
		);

		TypeRegistry::RegisterComponentType<AudioSource2DComponent>
		(
			NAME_OF(DYE::DYEditor::AudioSource2DComponent),
			ComponentTypeDescriptor
			{
				.Serialize = BuiltInComponentTypeFunctions::AudioSource2DComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::AudioSource2DComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::AudioSource2DComponent_DrawInspector,
				.GetDisplayName = []() { return "Audio Source 2D"; },
			}
		);

		TypeRegistry::RegisterComponentType<PlayAudioSourceOnInitializeComponent>
		(
			NAME_OF(DYE::DYEditor::PlayAudioSourceOnInitializeComponent),
			ComponentTypeDescriptor
			{
				.Serialize = BuiltInComponentTypeFunctions::SerializeEmptyComponent<PlayAudioSourceOnInitializeComponent>,
				.Deserialize = BuiltInComponentTypeFunctions::DeserializeEmptyComponent<PlayAudioSourceOnInitializeComponent>,
				.DrawInspector = BuiltInComponentTypeFunctions::PlayAudioSourceOnInitializeComponent_DrawInspector,
				.GetDisplayName = []() { return "Play Audio Source On Initialize"; },
			}
		);

		TypeRegistry::RegisterComponentType<WindowHandleComponent>
		(
			NAME_OF(DYE::DYEditor::WindowHandleComponent),
			ComponentTypeDescriptor
			{
				.Serialize = BuiltInComponentTypeFunctions::SerializeEmptyComponent<WindowHandleComponent>,
				.Deserialize = BuiltInComponentTypeFunctions::DeserializeEmptyComponent<WindowHandleComponent>,
				.DrawInspector = BuiltInComponentTypeFunctions::WindowHandleComponent_DrawInspector,
				.GetDisplayName = []() { return "Window Handle"; },
			}
		);

		TypeRegistry::RegisterComponentType<CreateWindowOnInitializeComponent>
		(
			NAME_OF(DYE::DYEditor::CreateWindowOnInitializeComponent),
			ComponentTypeDescriptor
			{
				.Serialize = BuiltInComponentTypeFunctions::CreateWindowOnInitializeComponent_Serialize,
				.Deserialize = BuiltInComponentTypeFunctions::CreateWindowOnInitializeComponent_Deserialize,
				.DrawInspector = BuiltInComponentTypeFunctions::CreateWindowOnInitializeComponent_DrawInspector,
				.GetDisplayName = []() { return "Create Window On Initialize"; },
			}
		);

		static ComputeLocalToWorldSystem _ComputeLocalToWorldSystem;
		TypeRegistry::RegisterSystem(ComputeLocalToWorldSystem::TypeName, &_ComputeLocalToWorldSystem);

		static Render2DSpriteSystem _Render2DSpriteSystem;
		TypeRegistry::RegisterSystem(Render2DSpriteSystem::TypeName, &_Render2DSpriteSystem);

		static RegisterCameraSystem _RegisterCameraSystem;
		TypeRegistry::RegisterSystem(RegisterCameraSystem::TypeName, &_RegisterCameraSystem);

		static ExecuteLoadSceneCommandSystem _ExecuteLoadSceneCommandSystem;
		TypeRegistry::RegisterSystem(ExecuteLoadSceneCommandSystem::TypeName, &_ExecuteLoadSceneCommandSystem);

		static AudioSystem _AudioSystem;
		TypeRegistry::RegisterSystem(AudioSystem::TypeName, &_AudioSystem);
		static PlayAudioSourceOnInitializeSystem _PlayAudioSourceOnInitializeSystem;
		TypeRegistry::RegisterSystem(PlayAudioSourceOnInitializeSystem::TypeName, &_PlayAudioSourceOnInitializeSystem);

		static ModifyWindowSystem _SetWindowPropertiesSystem;
		TypeRegistry::RegisterSystem(ModifyWindowSystem::TypeName, &_SetWindowPropertiesSystem);
		static CreateWindowOnInitializeSystem _CreateWindowOnInitializeSystem;
		TypeRegistry::RegisterSystem(CreateWindowOnInitializeSystem::TypeName, &_CreateWindowOnInitializeSystem);
		static CloseWindowOnTearDownSystem _CloseWindowOnTearDownSystem;
		TypeRegistry::RegisterSystem(CloseWindowOnTearDownSystem::TypeName, &_CloseWindowOnTearDownSystem);
	}

	ComponentTypeDescriptor TypeRegistry::GetComponentTypeDescriptor_NameComponent()
	{
		return s_NameComponentTypeDescriptor;
	}

	ComponentTypeDescriptor TypeRegistry::GetComponentTypeDescriptor_LocalTransformComponent()
	{
		return s_LocalTransformComponentTypeDescriptor;
	}

	ComponentTypeDescriptor TypeRegistry::GetComponentTypeDescriptor_LocalToWorldComponent()
	{
		return s_LocalToWorldComponentTypeDescriptor;
	}

	ComponentTypeDescriptor TypeRegistry::GetComponentTypeDescriptor_ParentComponent()
	{
		return s_ParentComponentTypeDescriptor;
	}

	ComponentTypeDescriptor TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent()
	{
		return s_ChildrenComponentTypeDescriptor;
	}
}