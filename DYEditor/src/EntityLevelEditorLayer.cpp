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

	void EntityLevelEditorLayer::drawEntityInspector(DYEntity::Entity& entity)
	{
		drawAddComponentButtonAndPopup(entity);
		drawAllComponents(entity);
	}

	void EntityLevelEditorLayer::drawAddComponentButtonAndPopup(DYEntity::Entity &entity)
	{
		static auto componentNamesAndFunctions = DYEntity::TypeRegistry::GetComponentTypesNamesAndFunctionCollections();

		// Draw a button aligned to the right,
		// And draw the popup menu if the button is pressed.
		char const* addComponentPopupId = "Add Component Menu Popup";
		ImVec2 const buttonSize = ImVec2 {150, 0};
		float const fullWidth = ImGui::GetWindowWidth();
		ImGui::SetCursorPosX(fullWidth - buttonSize.x);
		if (ImGui::Button("Add Component", buttonSize))
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
			ImGui::EndPopup();
		}
	}

	void EntityLevelEditorLayer::drawAllComponents(DYEntity::Entity &entity)
	{
		static auto componentNamesAndFunctions = DYEntity::TypeRegistry::GetComponentTypesNamesAndFunctionCollections();

		// Draw all components of m_Entity
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

			if (!ImGui::CollapsingHeader(name.c_str()))
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
				functions.DrawInspector(entity);
			}

			ImGui::Spacing();
		}
	}
}