#include "EntitySceneEditorLayer.h"

#include "BuiltInTypeRegister.h"
#include "UserTypeRegister.h"
#include "EditorSystem.h"

#include "ImGui/ImGuiUtil.h"

#include "Serialization/SerializedObjectFactory.h"

#include <unordered_set>
#include <stack>
#include <iostream>
#include <imgui.h>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	EntitySceneEditorLayer::EntitySceneEditorLayer() : LayerBase("Editor")
	{
	}

	void EntitySceneEditorLayer::OnAttach()
	{
		// DEBUGGING, Should be moved to EntityLevelEditorApplication so that both EntityLevelEditorLayer & EntityLevelRuntimeLayer could use it
		DYEditor::RegisterBuiltInTypes();
		DYEditor::RegisterUserTypes();

		// DEBUGGING
		m_Entity = m_World.CreateEntity();

		auto componentNamesAndFunctions = TypeRegistry::GetComponentTypesNamesAndFunctionCollections();
		for (auto& [name, functions] : componentNamesAndFunctions)
		{
			functions.Add(m_Entity);
		}


		// DEBUGGING
		std::optional<SerializedScene> serializedScene = SerializedObjectFactory::TryLoadSerializedSceneFromFile("assets\\Scenes\\TestScene.tscene");
		if (serializedScene.has_value())
		{
			auto serializedSystems = serializedScene->GetSerializedSystemHandles();
			DYE_LOG("Number Of Systems: %d\n", serializedSystems.size());
			for (auto &systemHandle: serializedSystems)
			{
				DYE_LOG("\tSystem: %s\n", systemHandle.TryGetTypeName()->c_str());
			}

			auto serializedEntities = serializedScene->GetSerializedEntityHandles();
			for (auto &entityHandle: serializedEntities)
			{
				auto serializedComponents = entityHandle.GetSerializedComponentHandles();
				DYE_LOG("Number Of Components: %d\n", serializedComponents.size());

				for (auto &componentHandle: serializedComponents)
				{
					DYE_LOG("\tComponent Type: %s\n", componentHandle.TryGetTypeName()->c_str());
				}
			}

			SerializedObjectFactory::ApplySerializedSceneToEmptyScene(serializedScene.value(), m_Scene);
		}
	}

	void EntitySceneEditorLayer::OnDetach()
	{
		TypeRegistry::ClearRegisteredComponentTypes();
		TypeRegistry::ClearRegisteredSystems();
	}

	void EntitySceneEditorLayer::OnImGui()
	{
		// Set a default size for the window in case it has never been opened before.
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Registered Systems"))
		{
			drawRegisteredSystems(m_World);
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Scene Hierarchy"))
		{
			drawSceneEntityHierarchyPanel(m_Scene, &m_CurrentSelectedEntity);
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Scene System"))
		{
			drawSceneSystemListPanel(m_Scene);
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Entity Inspector"))
		{
			//drawEntityInspector(m_Entity, TypeRegistry::GetComponentTypesNamesAndFunctionCollections());
			drawEntityInspector(m_CurrentSelectedEntity, TypeRegistry::GetComponentTypesNamesAndFunctionCollections());
		}
		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	bool EntitySceneEditorLayer::drawSceneEntityHierarchyPanel(Scene &scene, Entity *pCurrentSelectedEntity)
	{
		scene.World.ForEachEntity
		(
			[&pCurrentSelectedEntity](DYEntity::Entity& entity)
			{
				auto tryGetNameResult = entity.TryGetName();
				if (!tryGetNameResult.has_value())
				{
					// No name, skip it
					return ;
				}

				auto& name = tryGetNameResult.value();

				bool const isSelected = entity == *pCurrentSelectedEntity;

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

				bool const isNodeOpen = ImGui::TreeNodeEx((void*)(std::uint64_t) entity.GetID(), flags, name.c_str());
				if (ImGui::IsItemClicked())
				{
					*pCurrentSelectedEntity = entity;
				}

				if (isNodeOpen)
				{
					ImGui::TreePop();
				}
			}
		);

		return false;
	}

	bool EntitySceneEditorLayer::drawSceneSystemListPanel(Scene &scene)
	{
		bool changed = false;

		std::unordered_set<std::string> includedScenes(scene.SystemTypeNames.begin(), scene.SystemTypeNames.end());

		// Draw a 'Add System' button at the top of the inspector, and align it to the right side of the window.
		char const* addSystemPopupId = "Add System Menu Popup";
		ImVec2 const addButtonSize = ImVec2 {150, 0};
		float const fullAddButtonWidth = ImGui::GetWindowWidth();
		ImGui::SetCursorPosX(fullAddButtonWidth - addButtonSize.x);
		if (ImGui::Button("Add System", addButtonSize))
		{
			ImGui::OpenPopup(addSystemPopupId);
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Add a new system to the scene.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (ImGui::BeginPopup(addSystemPopupId))
		{
			if (ImGui::BeginListBox("##Add System List Box"))
			{
				for (auto const& [systemName, pSystemInstance] : TypeRegistry::GetSystemNamesAndInstances())
				{
					if (includedScenes.contains(systemName))
					{
						// The scene already has this system, skip it.
						continue;
					}

					if (ImGui::Selectable(systemName.c_str()))
					{
						// Add the system.
						scene.SystemTypeNames.push_back(systemName);
						changed = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndPopup();
		}

		std::stack<int> toBeRemovedSystemIndices;
		for (int i = 0; i < scene.SystemTypeNames.size(); ++i)
		{
			auto& systemName = scene.SystemTypeNames[i];

			ImGui::PushID(systemName.c_str());
			SystemBase* pSystemInstance = TypeRegistry::TryGetSystemInstance(systemName);
			bool const isRecognizedSystem = pSystemInstance != nullptr;
			char const* headerText = isRecognizedSystem? systemName.c_str() : "(Unrecognized System)";

			ImGui::AlignTextToFramePadding();

			bool isHeaderVisible = true;
			bool const isShown = ImGui::CollapsingHeader("", &isHeaderVisible, ImGuiTreeNodeFlags_AllowItemOverlap);
			bool const isRemoved = !isHeaderVisible;
			if (isRemoved)
			{
				changed = true;
				toBeRemovedSystemIndices.push(i);
				ImGui::PopID();
				continue;
			}

			static bool isEnabled = true;
			ImGui::SameLine();
			ImGui::Checkbox("##Checkbox", &isEnabled);
			ImGui::SameLine();
			ImGui::TextUnformatted(headerText);

			if (isShown)
			{
				if (isRecognizedSystem)
				{
					pSystemInstance->DrawInspector(scene.World);
				}
				else
				{
					ImGui::TextWrapped("System '%s' cannot be found in the TypeRegistry.", systemName.c_str());
				}
			}

			ImGui::PopID();
		}

		// Remove systems.
		while (!toBeRemovedSystemIndices.empty())
		{
			int const index = toBeRemovedSystemIndices.top(); toBeRemovedSystemIndices.pop();
			scene.SystemTypeNames.erase(scene.SystemTypeNames.begin() + index);
		}

		return false;
	}

	bool EntitySceneEditorLayer::drawEntityInspector(DYEntity::Entity &entity,
													 std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions)
	{
		if (!entity.IsValid())
		{
			return false;
		}

		bool changed = false;

		if (ImGui::Button("Save To TestPrefab.tprefab"))
		{
			auto serializedEntity = SerializedObjectFactory::CreateSerializedEntity(entity);
			SerializedObjectFactory::SaveSerializedEntityToFile(serializedEntity, "assets\\Scenes\\TestPrefab.tprefab");
		}

		// Draw a 'Add Component' button at the top of the inspector, and align it to the right side of the window.
		char const* addComponentPopupId = "Add Component Menu Popup";
		ImVec2 const addButtonSize = ImVec2 {150, 0};
		float const fullAddButtonWidth = ImGui::GetWindowWidth();
		ImGui::SetCursorPosX(fullAddButtonWidth - addButtonSize.x);
		if (ImGui::Button("Add Component", addButtonSize))
		{
			ImGui::OpenPopup(addComponentPopupId);
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Add a new component to the entity.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (ImGui::BeginPopup(addComponentPopupId))
		{
			if (ImGui::BeginListBox("##Add Component List Box"))
			{
				for (auto const& [name, functionCollections] : componentNamesAndFunctions)
				{
					if (functionCollections.Has(entity))
					{
						// The entity already has this component, skip it.
						continue;
					}

					if (ImGui::Selectable(name.c_str()))
					{
						// Add the component
						functionCollections.Add(entity);
						changed = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndPopup();
		}

		// Draw all components that the entity has.
		for (auto& [name, functions] : componentNamesAndFunctions)
		{
			if (functions.Has == nullptr)
			{
				ImGui::TextWrapped("Missing 'Has' function for component '%s'.", name.c_str());
				continue;
			}

			if (!functions.Has(entity))
			{
				continue;
			}

			bool isHeaderVisible = true;
			bool const showInspector = ImGui::CollapsingHeader(name.c_str(), &isHeaderVisible);

			bool const isRemoved = !isHeaderVisible;
			if (isRemoved)
			{
				// Remove the component
				functions.Remove(entity);
				changed = true;
				continue;
			}

			if (!showInspector)
			{
				continue;
			}

			if (functions.DrawInspector == nullptr)
			{
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextWrapped("Missing 'DrawInspector' function for component '%s'. "
									   "It's likely that the DrawInspectorFunction is not assigned when TypeRegistry::registerComponentType is called.", name.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}
			else
			{
				ImGui::PushID(name.c_str());
				changed |= functions.DrawInspector(entity);
				ImGui::PopID();
			}

			ImGui::Spacing();
		}

		return changed;
	}

	void EntitySceneEditorLayer::drawRegisteredSystems(DYEntity::World &world)
	{
		static auto systemNamesAndInstances = TypeRegistry::GetSystemNamesAndInstances();

		for (auto& [name, systemBasePtr] : systemNamesAndInstances)
		{
			ImGui::PushID(name.c_str());
			bool const show = ImGui::CollapsingHeader(name.c_str());

			if (show)
			{
				ImGui::TextWrapped(ExecutionPhaseToString(systemBasePtr->GetPhase()).c_str());
				if (ImGui::Button("Execute System Function"))
				{
					systemBasePtr->Execute(world, { .Phase = ExecutionPhase::Initialize });
				}
				systemBasePtr->DrawInspector(world);
			}
			ImGui::PopID();
		}
	}
}