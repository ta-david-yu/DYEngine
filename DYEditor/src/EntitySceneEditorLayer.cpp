#include "EntitySceneEditorLayer.h"

#include "BuiltInTypeRegister.h"
#include "UserTypeRegister.h"
#include "EditorSystem.h"

#include "ImGui/ImGuiUtil.h"

#include "Serialization/SerializedObjectFactory.h"

#include <imgui.h>
#include <iostream>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	EntitySceneEditorLayer::EntitySceneEditorLayer() : LayerBase("Editor")
	{
		std::optional<SerializedScene> serializedScene = SerializedObjectFactory::GetSerializedSceneFromFile("assets\\Scenes\\TestScene.tscene");
		if (serializedScene)
		{
			auto serializedSystems = serializedScene->GetSerializedSystemHandles();
			DYE_LOG("Number Of Systems: %d\n", serializedSystems.size());
			for (auto& systemHandle : serializedSystems)
			{
				DYE_LOG("\tSystem: %s\n", systemHandle.GetTypeName()->c_str());
			}

			auto serializedEntities = serializedScene->GetSerializedEntityHandles();
			for (auto& entityHandle : serializedEntities)
			{
				auto serializedComponents = entityHandle.GetSerializedComponentHandles();
				DYE_LOG("Number Of Components: %d\n", serializedComponents.size());

				for (auto& componentHandle : serializedComponents)
				{
					DYE_LOG("\tComponent Type: %s\n", componentHandle.TryGetTypeName()->c_str());
				}
			}
		}
	}

	void EntitySceneEditorLayer::OnAttach()
	{
		// DEBUGGING, Should be moved to EntityLevelEditorApplication so that both EntityLevelEditorLayer & EntityLevelRuntimeLayer could use it
		DYEditor::RegisterBuiltInTypes();
		DYEditor::RegisterUserTypes();

		// DEBUGGING
		m_Entity = m_World.CreateEntity();

		m_ComponentTypeAndFunctions = TypeRegistry::GetComponentTypesNamesAndFunctionCollections();
		for (auto& [name, functions] : m_ComponentTypeAndFunctions)
		{
			functions.Add(m_Entity);
		}
	}

	void EntitySceneEditorLayer::OnDetach()
	{
		TypeRegistry::ClearRegisteredComponentTypes();
		TypeRegistry::ClearRegisteredSystems();
	}

	void EntitySceneEditorLayer::OnImGui()
	{
		ImGui::ShowDemoWindow();

		// Set a default size for the window in case it has never been opened before.
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Component List"))
		{
			drawEntityInspector(m_Entity, m_ComponentTypeAndFunctions);
		}
		ImGui::End();

		if (ImGui::Begin("Registered Systems"))
		{
			drawRegisteredSystems(m_World);
		}
		ImGui::End();
	}

	bool EntitySceneEditorLayer::drawEntityInspector(DYEntity::Entity &entity,
													 std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions)
	{
		bool changed = false;

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
			ImGui::TextUnformatted("Add a new component to this entity.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (ImGui::BeginPopup(addComponentPopupId))
		{
			if (ImGui::BeginListBox("##Add Component List Box"))
			{
				for (int componentIndex = 0; componentIndex < componentNamesAndFunctions.size(); componentIndex++)
				{
					auto const& name = componentNamesAndFunctions[componentIndex].first;
					auto const& functionCollections = componentNamesAndFunctions[componentIndex].second;

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

			bool showHeader = true;
			bool const showInspector = ImGui::CollapsingHeader(name.c_str(), &showHeader);

			bool const isRemoved = !showHeader;
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