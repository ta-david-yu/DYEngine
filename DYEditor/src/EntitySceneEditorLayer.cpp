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
		drawMainMenuBar();

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
			#pragma unroll
			for (int phaseIndex = static_cast<int>(ExecutionPhase::Initialize); phaseIndex <= static_cast<int>(ExecutionPhase::TearDown); phaseIndex++)
			{
				auto const phase = static_cast<ExecutionPhase>(phaseIndex);
				std::string const& phaseId = CastExecutionPhaseToString(phase);
				ImGui::PushID(phaseId.c_str());
				ImGui::Separator();
				bool const showSystems = ImGui::CollapsingHeader(phaseId.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
				if (showSystems)
				{
					drawSceneSystemListPanel(m_Scene, m_Scene.GetSystemDescriptorsOfPhase(phase),
											 [phase](std::string const &systemName, SystemBase const *pInstance)
											 {
												 return pInstance->GetPhase() == phase;
											 });
				}
				ImGui::PopID();
			}

			ImGui::Separator();
			if (!m_Scene.UnrecognizedSystems.empty())
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.000f, 0.000f, 0.000f, 0.310f));
				bool const showUnrecognizedSystems = ImGui::CollapsingHeader("Unrecognized Systems");
				ImGui::PopStyleColor();
				if (showUnrecognizedSystems)
				{
					for (auto const &descriptor: m_Scene.UnrecognizedSystems)
					{
						ImGui::PushID(descriptor.Name.c_str());
						ImGui::Text("System '%s' is unknown in the TypeRegistry.", descriptor.Name.c_str());
						char const *popupId = "Unknown system popup";
						if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						{
							ImGui::OpenPopup(popupId);
						}

						if (ImGui::BeginPopup(popupId))
						{
							if (ImGui::Button("Copy System Name"))
							{
								ImGui::SetClipboardText(descriptor.Name.c_str());
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
						ImGui::PopID();
					}
				}
			}
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Entity Inspector"))
		{
			if (m_CurrentSelectedEntity.IsValid() && ImGui::Button("Save To TestPrefab.tprefab"))
			{
				auto serializedEntity = SerializedObjectFactory::CreateSerializedEntity(m_CurrentSelectedEntity);
				SerializedObjectFactory::SaveSerializedEntityToFile(serializedEntity, "assets\\Scenes\\TestPrefab.tprefab");
			}
			ImGui::Separator();

			drawEntityInspector(m_CurrentSelectedEntity, TypeRegistry::GetComponentTypesNamesAndFunctionCollections());
		}
		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	void EntitySceneEditorLayer::drawMainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
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

	template<typename Func> requires std::predicate<Func, std::string const&, SystemBase const*>
	bool EntitySceneEditorLayer::drawSceneSystemListPanel(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate)
	{
		bool changed = false;

		// Create a set for faster lookup of already included systems.
		std::unordered_set<std::string> alreadyIncludedSystems;
		alreadyIncludedSystems.reserve(systemDescriptors.size());
		std::for_each(systemDescriptors.begin(), systemDescriptors.end(),
					  	[&alreadyIncludedSystems](SystemDescriptor const& descriptor)
					  	{
							alreadyIncludedSystems.insert(descriptor.Name);
					 	});

		ImGui::TextWrapped("Number Of Systems: %d", systemDescriptors.size());
		ImGui::SameLine();

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
					if (alreadyIncludedSystems.contains(systemName))
					{
						// The scene already has this system, skip it.
						continue;
					}

					if (!addSystemFilterPredicate(systemName, pSystemInstance))
					{
						continue;
					}

					if (ImGui::Selectable(systemName.c_str()))
					{
						// Add the system.
						systemDescriptors.push_back
						(
							SystemDescriptor
								{
									.Name = systemName,
									.Group = NO_SYSTEM_GROUP_ID
								}
						);
						changed = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndPopup();
		}

		std::stack<int> toBeRemovedSystemIndices;
		for (int i = 0; i < systemDescriptors.size(); ++i)
		{
			auto& systemDescriptor = systemDescriptors[i];

			ImGui::PushID(systemDescriptor.Name.c_str());
			SystemBase* pSystemInstance = TypeRegistry::TryGetSystemInstance(systemDescriptor.Name);
			bool const isRecognizedSystem = pSystemInstance != nullptr;
			char const* headerText = isRecognizedSystem ? systemDescriptor.Name.c_str() : "(Unrecognized System)";

			ImGui::AlignTextToFramePadding();

			bool isHeaderVisible = true;
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.000f, 0.215f, 0.470f, 0.310f));
			bool const isShown = ImGui::CollapsingHeader("", &isHeaderVisible, ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::PopStyleColor();
			bool const isRemoved = !isHeaderVisible;
			if (isRemoved)
			{
				changed = true;
				toBeRemovedSystemIndices.push(i);
				ImGui::PopID();
				continue;
			}

			static bool isEnabled = true;
			// Draw enabled toggle.
			ImGui::SameLine();
			ImGui::Checkbox("##Checkbox", &isEnabled);

			// Draw header text (system name most likely).
			ImGui::SameLine();
			ImGui::TextUnformatted(headerText);

			// Draw move up & move down buttons, from right to left.
			bool const isTheFirst = i == 0;
			bool const isTheLast = i == systemDescriptors.size() - 1;
			float const offsetToRight = ImGui::GetFrameHeightWithSpacing();
			float const fullReorderButtonWidth = ImGui::GetContentRegionAvail().x;
			if (!isTheFirst)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(fullReorderButtonWidth - offsetToRight * 2);    // * 2 because: close button + up button (itself)
				if (ImGui::ArrowButton("##up", ImGuiDir_Up))
				{
					// Swap with the previous system and return right away.
					int const otherSystemIndex = i - 1;
					auto const otherSystemDescriptor = systemDescriptors[otherSystemIndex];
					systemDescriptors[otherSystemIndex] = systemDescriptor;
					systemDescriptors[i] = otherSystemDescriptor;
					ImGui::PopID();
					return true;
				}
			}
			if (!isTheLast)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(fullReorderButtonWidth - offsetToRight * 3);    // * 3 because: close button + up button + down button (itself)
				if (ImGui::ArrowButton("##down", ImGuiDir_Down))
				{
					// Swap with the next system and return right away.
					int const otherSystemIndex = i + 1;
					auto const otherSystemDescriptor = systemDescriptors[otherSystemIndex];
					systemDescriptors[otherSystemIndex] = systemDescriptor;
					systemDescriptors[i] = otherSystemDescriptor;
					ImGui::PopID();
					return true;
				}
			}

			if (isShown)
			{
				if (systemDescriptor.Group != NO_SYSTEM_GROUP_ID)
				{
					ImGuiUtil::DrawReadOnlyTextWithLabel("Group", scene.SystemGroupNames[systemDescriptor.Group]);
				}
				if (isRecognizedSystem)
				{
					pSystemInstance->DrawInspector(scene.World);
				}
				else
				{
					ImGui::TextWrapped("System '%s' cannot be found in the TypeRegistry.", systemDescriptor.Name.c_str());
				}
			}

			ImGui::PopID();
		}

		// Remove systems.
		while (!toBeRemovedSystemIndices.empty())
		{
			int const index = toBeRemovedSystemIndices.top(); toBeRemovedSystemIndices.pop();
			systemDescriptors.erase(systemDescriptors.begin() + index);
		}

		return changed;
	}

	bool EntitySceneEditorLayer::drawEntityInspector(DYEntity::Entity &entity,
													 std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions)
	{
		if (!entity.IsValid())
		{
			return false;
		}

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
				ImGui::TextWrapped(CastExecutionPhaseToString(systemBasePtr->GetPhase()).c_str());
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