#include "SceneEditorLayer.h"

#include "SceneRuntimeLayer.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/WindowManager.h"
#include "ImGui/EditorImGuiUtil.h"
#include "ImGui/ImGuiUtil.h"

#include <filesystem>
#include <unordered_set>
#include <stack>
#include <iostream>
#include <imgui.h>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	SceneEditorLayer::SceneEditorLayer() : LayerBase("Editor")
	{
	}

	void SceneEditorLayer::OnAttach()
	{
		// DEBUGGING, Should be moved to DYEditorApplication so that both EditorLayer & RuntimeLayer could use it
		DYEditor::RegisterBuiltInTypes();
		DYEditor::RegisterUserTypes();

		m_SceneViewCamera.Properties.TargetWindowID = WindowManager::GetMainWindow()->GetWindowID();
	}

	void SceneEditorLayer::OnDetach()
	{
		TypeRegistry::ClearRegisteredComponentTypes();
		TypeRegistry::ClearRegisteredSystems();
	}

	void SceneEditorLayer::OnRender()
	{
		DYE::RenderPipelineManager::RegisterCameraForNextRender(m_SceneViewCamera);
	}

	void SceneEditorLayer::OnEvent(Event &event)
	{
	}

	void SceneEditorLayer::OnImGui()
	{
		if (!m_RuntimeLayer)
		{
			return;
		}

		Scene &activeScene = m_RuntimeLayer->ActiveMainScene;

		drawMainMenuBar(activeScene, m_CurrentSceneFilePath);

		// Set a default size for the window in case it has never been opened before.
		const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Scene View Camera"))
		{
			ImGui::PushID("Scene View Camera");
			ImGuiUtil::DrawVector3Control("Position", m_SceneViewCamera.Position);
			ImGuiUtil::DrawCameraPropertiesControl("Properties", m_SceneViewCamera.Properties);
			ImGui::PopID();
		}
		ImGui::End();


		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
								ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Registered Systems"))
		{
			drawRegisteredSystems(activeScene.World);
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
								ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Scene System"))
		{
			#pragma unroll
			for (int phaseIndex = static_cast<int>(ExecutionPhase::Initialize);
				 phaseIndex <= static_cast<int>(ExecutionPhase::TearDown); phaseIndex++)
			{
				auto const phase = static_cast<ExecutionPhase>(phaseIndex);
				std::string const &phaseId = CastExecutionPhaseToString(phase);
				ImGui::PushID(phaseId.c_str());
				ImGui::Separator();
				bool const showSystems = ImGui::CollapsingHeader(phaseId.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
				if (showSystems)
				{
					drawSceneSystemListPanel(activeScene, activeScene.GetSystemDescriptorsOfPhase(phase),
											 [phase](std::string const &systemName, SystemBase const *pInstance)
											 {
												 return pInstance->GetPhase() == phase;
											 });
				}
				ImGui::PopID();
			}

			ImGui::Separator();
			if (!activeScene.UnrecognizedSystems.empty())
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.000f, 0.000f, 0.000f, 0.310f));
				bool const showUnrecognizedSystems = ImGui::CollapsingHeader("Unrecognized Systems");
				ImGui::PopStyleColor();
				if (showUnrecognizedSystems)
				{
					if (ImGui::BeginTable("Unrecognized System Table", 1, ImGuiTableFlags_RowBg))
					{
						int indexToRemove = -1;
						for (int i = 0; i < activeScene.UnrecognizedSystems.size(); ++i)
						{
							auto const &descriptor = activeScene.UnrecognizedSystems[i];

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::PushID(descriptor.Name.c_str());

							char label[128];
							sprintf(label, "System '%s' is unknown in the TypeRegistry.", descriptor.Name.c_str());
							ImGui::Bullet();
							ImGui::Selectable(label, false);

							//ImGui::Text("System '%s' is unknown in the TypeRegistry.", descriptor.Name.c_str());
							char const *popupId = "Unknown system popup";
							if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
							{
								ImGui::OpenPopup(popupId);
							}

							if (ImGui::BeginPopup(popupId))
							{
								if (ImGui::Selectable("Copy Name"))
								{
									ImGui::SetClipboardText(descriptor.Name.c_str());
								}
								if (ImGui::Selectable("Delete"))
								{
									indexToRemove = i;
								}
								ImGui::EndPopup();
							}
							ImGui::PopID();
						}

						if (indexToRemove != -1)
						{
							activeScene.UnrecognizedSystems.erase(
								activeScene.UnrecognizedSystems.begin() + indexToRemove);
						}

						ImGui::EndTable();
					}
				}
			}
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
								ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Scene Hierarchy"))
		{
			drawSceneEntityHierarchyPanel(activeScene, &m_CurrentlySelectedEntityInHierarchyPanel);
		}
		ImGui::End();

		// Set a default size for the window in case it has never been opened before.
		main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
								ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Entity Inspector"))
		{
			if (m_CurrentlySelectedEntityInHierarchyPanel.IsValid() && ImGui::Button("Save To TestPrefab.tprefab"))
			{
				auto serializedEntity = SerializedObjectFactory::CreateSerializedEntity(
					m_CurrentlySelectedEntityInHierarchyPanel);
				SerializedObjectFactory::SaveSerializedEntityToFile(serializedEntity,
																	"assets\\Scenes\\TestPrefab.tprefab");
			}
			ImGui::Separator();

			drawEntityInspector(m_CurrentlySelectedEntityInHierarchyPanel,
								TypeRegistry::GetComponentTypesNamesAndFunctionCollections());
		}
		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	void SceneEditorLayer::drawMainMenuBar(Scene &currentScene, std::filesystem::path &currentScenePathContext)
	{
		bool openLoadSceneFilePathPopup = false;
		bool openSaveSceneFilePathPopup = false;

		char const* loadScenePopupId = "Select a scene file (*.tscene)";
		char const* saveScenePopupId = "Save scene file as... ";

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					currentScenePathContext.clear();
					currentScene.Clear();
				}

				if (ImGui::MenuItem("Open Scene"))
				{
					// We store a flag here and delay opening the popup
					// because MenuItem is Selectable and Selectable by default calls CloseCurrentPopup().
					openLoadSceneFilePathPopup = true;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save Scene"))
				{
					if (currentScenePathContext.empty())
					{
						// If the context filepath is empty, it's the same as 'Save Scene as...'.
						openSaveSceneFilePathPopup = true;
					}
					else
					{
						auto serializedScene = SerializedObjectFactory::CreateSerializedScene(currentScene);
						SerializedObjectFactory::SaveSerializedSceneToFile(serializedScene, currentScenePathContext);
					}
				}

				if (ImGui::MenuItem("Save Scene as..."))
				{
					// We store a flag here and delay opening the popup
					// because MenuItem is Selectable and Selectable by default calls CloseCurrentPopup().
					openSaveSceneFilePathPopup = true;
				}

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

		static std::filesystem::path sceneFilePath = "";

		// Draw open/load scene file path popup.
		if (openLoadSceneFilePathPopup)
		{
			ImGuiUtil::OpenFilePathPopup(loadScenePopupId, "assets", currentScenePathContext, {".tscene" });
		}
		ImGuiUtil::FilePathPopupResult loadFilePathResult = ImGuiUtil::DrawFilePathPopup(loadScenePopupId, sceneFilePath, ImGuiUtil::FilePathPopupParameters
			{
				.IsSaveFilePanel = false
			});
		if (loadFilePathResult == ImGuiUtil::FilePathPopupResult::Confirm)
		{
			currentScene.Clear();
			std::optional<SerializedScene> serializedScene = SerializedObjectFactory::TryLoadSerializedSceneFromFile(sceneFilePath);
			if (serializedScene.has_value())
			{
				SerializedObjectFactory::ApplySerializedSceneToEmptyScene(serializedScene.value(), currentScene);
				currentScenePathContext = sceneFilePath;
			}
		}

		// Draw save scene file path popup.
		if (openSaveSceneFilePathPopup)
		{
			ImGuiUtil::OpenFilePathPopup(saveScenePopupId, "assets", currentScenePathContext, {".tscene"});
		}
		ImGuiUtil::FilePathPopupResult saveFilePathResult = ImGuiUtil::DrawFilePathPopup(saveScenePopupId, sceneFilePath, ImGuiUtil::FilePathPopupParameters
			{
				.IsSaveFilePanel = true,
				.SaveFileExtension = ".tscene",
				.ShowOverwritePopupOnConfirmSave = true,
			});

		if (saveFilePathResult == ImGuiUtil::FilePathPopupResult::Confirm)
		{
			currentScene.Name = sceneFilePath.filename().stem().string();
			auto serializedScene = SerializedObjectFactory::CreateSerializedScene(currentScene);
			SerializedObjectFactory::SaveSerializedSceneToFile(serializedScene, sceneFilePath);
			currentScenePathContext = sceneFilePath;
		}
	}

	bool SceneEditorLayer::drawSceneEntityHierarchyPanel(Scene &scene, Entity *pCurrentSelectedEntity)
	{
		// Draw scene hierarchy context menu.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Create Empty"))
			{
				scene.World.CreateEntity("Entity");
			}
			ImGui::EndPopup();
		}

		// Draw scene name as title.
		if (!scene.Name.empty())
		{
			ImGui::SeparatorText(scene.Name.c_str());
		}
		else
		{
			ImGui::SeparatorText("Untitled");
		}

		// Draw all entities.
		scene.World.ForEachEntity
		(
			[&scene, &pCurrentSelectedEntity](DYEntity::Entity& entity)
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
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					*pCurrentSelectedEntity = entity;
				}

				// Draw entity context menu.
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::Selectable("Delete"))
					{
						scene.World.DestroyEntity(entity);
					}
					ImGui::EndPopup();
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
	bool SceneEditorLayer::drawSceneSystemListPanel(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate)
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
		float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY? ImGui::GetWindowScrollbarRect(ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;
		float const availableWidthForAddButton = ImGui::GetWindowWidth() - scrollBarWidth;
		ImGui::SetCursorPosX(availableWidthForAddButton - addButtonSize.x);
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
									.Group = NoSystemGroupID,
									.IsEnabled = true,
									.Instance = pSystemInstance
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

		int indexToRemove = -1;
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
				indexToRemove = i;
				ImGui::PopID();
				continue;
			}

			// Draw enabled toggle.
			ImGui::SameLine();
			ImGui::Checkbox("##IsEnabled", &systemDescriptor.IsEnabled);

			// Draw header text (system name most likely).
			ImGui::SameLine();
			ImGui::TextUnformatted(headerText);

			// Draw move up & move down buttons, from right to left.
			bool const isTheFirst = i == 0;
			bool const isTheLast = i == systemDescriptors.size() - 1;
			float const offsetToRight = ImGui::GetFrameHeightWithSpacing();
			float const availableWidthForReorderButtons = ImGui::GetWindowWidth() - scrollBarWidth;
			if (!isTheFirst)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(availableWidthForReorderButtons - offsetToRight * 2);    // * 2 because: close button + up button (itself)
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
				ImGui::SetCursorPosX(availableWidthForReorderButtons - offsetToRight * 3);    // * 3 because: close button + up button + down button (itself)
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
				if (systemDescriptor.Group != NoSystemGroupID)
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

				ImGui::Spacing();
			}

			ImGui::PopID();
		}

		// Remove systems.
		if (indexToRemove != -1)
		{
			systemDescriptors.erase(systemDescriptors.begin() + indexToRemove);
		}

		return changed;
	}

	bool SceneEditorLayer::drawEntityInspector(DYEntity::Entity &entity,
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
		float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY? ImGui::GetWindowScrollbarRect(ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;
		float const availableWidthForAddButton = ImGui::GetWindowWidth() - scrollBarWidth;
		ImGui::SetCursorPosX(availableWidthForAddButton - addButtonSize.x);
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
			bool showComponentInspector = true;
			if (functions.DrawHeader == nullptr)
			{
				ImGuiTreeNodeFlags const flags = ImGuiTreeNodeFlags_DefaultOpen;
				showComponentInspector = ImGui::CollapsingHeader(name.c_str(), &isHeaderVisible, flags);
			}
			else
			{
				// Use custom header if provided.
				showComponentInspector = functions.DrawHeader(entity, isHeaderVisible, changed, name);
			}

			bool const isRemoved = !isHeaderVisible;
			if (isRemoved)
			{
				// Remove the component
				functions.Remove(entity);
				changed = true;
				continue;
			}

			if (!showComponentInspector)
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

	void SceneEditorLayer::drawRegisteredSystems(DYEntity::World &world)
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