#include "EntityLevelEditorLayer.h"

#include "TypeRegistry.h"
#include "Components.h"
#include "ImGui/ImGuiUtil.h"

#include <imgui.h>

namespace DYE::DYEditor
{
	EntityLevelEditorLayer::EntityLevelEditorLayer() : LayerBase("Editor")
	{
		DYEntity::RegisterBuiltInComponentTypes();

		m_Entity = m_World.CreateEntity();

		auto componentFunctions = DYEntity::TypeRegistry::GetComponentTypeFunctionCollections();
		for (auto functions : componentFunctions)
		{
			functions.Add(m_Entity);
		}
		//m_Entity.AddComponent<DYEntity::TransformComponent>();
	}

	void EntityLevelEditorLayer::OnImGui()
	{
		ImGui::ShowDemoWindow();

		// Set a default size for the window in case it has never been opened before.
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("Component List"))
		{
			ImGui::End();
			return;
		}

		drawEntityInspector(m_Entity);

		ImGui::End();
	}

	bool EntityLevelEditorLayer::drawEntityInspector(DYEntity::Entity& entity)
	{
		static auto componentNamesAndFunctions = DYEntity::TypeRegistry::GetComponentTypesNamesAndFunctionCollections();

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
		for (auto [name, functions] : componentNamesAndFunctions)
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
									   "It's likely that the DrawInspectorFunction is not assigned when TypeRegistry::RegisterComponentType is called.", name.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}
			else
			{
				changed |= functions.DrawInspector(entity);
			}

			ImGui::Spacing();
		}

		return changed;
	}
}