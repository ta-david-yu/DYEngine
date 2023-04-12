#include "SceneEditorLayer.h"

#include "SceneRuntimeLayer.h"
#include "Core/ApplicationState.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"
#include "NameComponent.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/WindowManager.h"
#include "Input/InputManager.h"
#include "Event/MouseEvent.h"
#include "Util/Time.h"
#include "EditorConfig.h"
#include "ImGui/EditorWindowManager.h"
#include "ImGui/EditorImGuiUtil.h"
#include "ImGui/ImGuiUtil.h"

#include <filesystem>
#include <unordered_set>
#include <stack>
#include <iostream>
#include <imgui.h>
#include <imgui_stdlib.h>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	constexpr char const* k_SceneHierarchyWindowId = "Scene Hierarchy";
	constexpr char const* k_SceneSystemWindowId = "Scene System";
	constexpr char const* k_EntityInspectorWindowId = "Entity Inspector";
	constexpr char const* k_SceneViewWindowId = "Scene View";

	SceneEditorLayer::SceneEditorLayer() : LayerBase("Editor")
	{
	}

	void SceneEditorLayer::OnAttach()
	{
		// DEBUGGING, Should be moved to DYEditorApplication so that both EditorLayer & RuntimeLayer could use it
		DYEditor::RegisterBuiltInTypes();
		DYEditor::RegisterUserTypes();

		EditorWindowManager::RegisterEditorWindow(
			RegisterEditorWindowParameters
			{
				.Name = "ImGui Demo",
				.isConfigOpenByDefault = false
			},
			[](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
			{
				ImGui::ShowDemoWindow(pIsOpen);
			}
		);

		EditorWindowManager::RegisterEditorWindow(
			RegisterEditorWindowParameters
				{
					.Name = "Window Manager",
					.isConfigOpenByDefault = false
				},
			[](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
			{
				WindowManager::DrawWindowManagerImGui(pIsOpen);
			}
		);

		EditorWindowManager::RegisterEditorWindow(
			RegisterEditorWindowParameters
				{
					.Name = "Input Manager",
					.isConfigOpenByDefault = false
				},
			[](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
			{
				INPUT.DrawInputManagerImGui(pIsOpen);
			}
		);

		// DEBUGGING Camera & Window Setup
		m_SceneViewCamera.Properties.TargetWindowIndex = WindowManager::MainWindowIndex;
		m_SceneViewCamera.Position = {0, 0, 10};

		auto windowPtr = WindowManager::CreateWindow(WindowProperty("Test Window", 640, 480));
		windowPtr->SetContext(WindowManager::GetMainWindow()->GetContext());
	}

	void SceneEditorLayer::OnDetach()
	{
		EditorWindowManager::ClearRegisteredEditorWindows();
		TypeRegistry::ClearRegisteredComponentTypes();
		TypeRegistry::ClearRegisteredSystems();
	}

	void SceneEditorLayer::OnUpdate()
	{
		if (INPUT.GetKey(KeyCode::W))
		{
			m_SceneViewCamera.Position.y += m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
		}
		if (INPUT.GetKey(KeyCode::S))
		{
			m_SceneViewCamera.Position.y -= m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
		}
		if (INPUT.GetKey(KeyCode::D))
		{
			m_SceneViewCamera.Position.x += m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
		}
		if (INPUT.GetKey(KeyCode::A))
		{
			m_SceneViewCamera.Position.x -= m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
		}

		if (INPUT.GetMouseButton(MouseButton::Middle))
		{
			auto mouseDelta = INPUT.GetGlobalMouseDelta();

			glm::vec2 panMove = mouseDelta; panMove *= m_CameraMousePanMoveUnitPerSecond * TIME.DeltaTime();
			float const multiplierBasedOnCameraDistance = m_SceneViewCamera.Properties.OrthographicSize / 10.0f;
			panMove *= multiplierBasedOnCameraDistance;

			// Invert x so when the mouse cursor moves left, the camera moves right.
			// We don't need to invert y because the Y direction in screen coordinate (i.e. mouse delta) is opposite to 2d world coordinate.
			m_SceneViewCamera.Position += glm::vec3 {-panMove.x, panMove.y, 0};
		}
	}

	void SceneEditorLayer::OnEvent(Event &event)
	{
		if (event.GetEventType() == EventType::MouseScroll)
		{
			auto mouseScrolledEvent = (MouseScrolledEvent&) event;
			m_SceneViewCamera.Properties.OrthographicSize -= TIME.DeltaTime() * m_CameraOrthographicSizeZoomSpeedMultiplier * mouseScrolledEvent.GetY();
			if (m_SceneViewCamera.Properties.OrthographicSize < 0.1f)
			{
				m_SceneViewCamera.Properties.OrthographicSize = 0.1f;
			}
		}
	}

	void SceneEditorLayer::OnRender()
	{
		DYE::RenderPipelineManager::RegisterCameraForNextRender(m_SceneViewCamera);
	}

	void SceneEditorLayer::OnImGui()
	{
		if (!m_RuntimeLayer)
		{
			return;
		}

		Scene &activeScene = m_RuntimeLayer->ActiveMainScene;

		// Create the main editor window that can be docked by other editor windows.
		// By default, it also hosts all the major windows such as Scene Hierarchy Panel, Entity Inspector & Scene View etc.
		ImGuiWindowFlags mainEditorWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGuiDockNodeFlags mainEditorWindowDockSpaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DYEditor Window", nullptr, mainEditorWindowFlags);
		ImGui::PopStyleVar();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockSpaceId = ImGui::GetID("DYEditor Window DockSpace");
			if (ImGui::DockBuilderGetNode(dockSpaceId) == nullptr)
			{
				setEditorWindowDefaultLayout(dockSpaceId);
			}
			ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), mainEditorWindowDockSpaceFlags);
		}

		drawEditorWindowMenuBar(activeScene, m_CurrentSceneFilePath);
		ImGuiViewport const *mainEditorWindowViewport = ImGui::GetWindowViewport();
		ImGui::End();

		// Draw other extra windows here.
		EditorWindowManager::DrawEditorWindows(mainEditorWindowViewport);

		// Draw all the major editor windows.
		ImGuiWindowFlags const sceneViewWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground;
		if (ImGui::Begin(k_SceneViewWindowId, nullptr, sceneViewWindowFlags))
		{
			drawSceneView(m_SceneViewCamera);
		}
		ImGui::End();

		if (ImGui::Begin(k_SceneSystemWindowId))
		{
			#pragma unroll
			for (int phaseIndex = static_cast<int>(ExecutionPhase::Initialize);
				 phaseIndex <= static_cast<int>(ExecutionPhase::TearDown); phaseIndex++)
			{
				auto const phase = static_cast<ExecutionPhase>(phaseIndex);
				bool const isPhaseRunning = ApplicationState::IsPlaying() || (phase == ExecutionPhase::Render || phase == ExecutionPhase::PostRender);

				std::string const &phaseId = CastExecutionPhaseToString(phase);
				auto& systemDescriptors = activeScene.GetSystemDescriptorsOfPhase(phase);

				ImGui::PushID(phaseId.c_str());
				ImGui::Separator();

				if (!isPhaseRunning)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
				}
				bool const showSystems = ImGui::CollapsingHeader(phaseId.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
				if (!systemDescriptors.empty())
				{
					// Draw number of systems in this phase category on the header for easier read.
					ImGui::SameLine();
					ImGui::Text("(%d)", systemDescriptors.size());
				}
				if (!isPhaseRunning)
				{
					ImGui::PopStyleColor();
				}

				if (phase == ExecutionPhase::Render || phase == ExecutionPhase::PostRender)
				{
					ImGui::SameLine();
					ImGuiUtil::DrawHelpMarker("Render & PostRender systems are executed in both Play Mode & Edit Mode.");
				}
				
				if (showSystems)
				{
					drawSceneSystemList(activeScene, systemDescriptors,
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

		if (ImGui::Begin(k_SceneHierarchyWindowId))
		{
			drawSceneEntityHierarchyPanel(activeScene, &m_CurrentlySelectedEntityInHierarchyPanel);
		}
		ImGui::End();

		if (ImGui::Begin(k_EntityInspectorWindowId))
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
	}
	void SceneEditorLayer::setEditorWindowDefaultLayout(ImGuiID dockSpaceId)
	{
		ImGui::DockBuilderRemoveNode(dockSpaceId);
		ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockSpaceId, ImGui::GetWindowViewport()->Size);

		/*
		 * We want to create a dock space layout like this:
		  ____ ____ ____ ____
		 | L1 |         |  R |
		 |____|    C    |    |
		 | L2 |         |    |
		 |____|____ ____|____|
		 * L1: Scene Hierarchy
		 * L2: Scene System
		 * C : Scene View
		 * R : Entity Inspector
		 */

		ImGuiID centerId = dockSpaceId;
		ImGuiID left1_Id = ImGui::DockBuilderSplitNode(centerId, ImGuiDir_Left, 0.25f, nullptr, &centerId);
		ImGuiID left2_Id = ImGui::DockBuilderSplitNode(left1_Id, ImGuiDir_Down, 0.5f, nullptr, &left1_Id);
		ImGuiID rightId = ImGui::DockBuilderSplitNode(centerId, ImGuiDir_Right, 0.33f, nullptr, &centerId);

		ImGui::DockBuilderDockWindow(k_SceneHierarchyWindowId, left1_Id);
		ImGui::DockBuilderDockWindow(k_SceneSystemWindowId, left2_Id);
		ImGui::DockBuilderDockWindow(k_EntityInspectorWindowId, rightId);
		ImGui::DockBuilderDockWindow(k_SceneViewWindowId, centerId);

		ImGui::DockBuilderFinish(dockSpaceId);
	}

	void SceneEditorLayer::drawSceneView(Camera &sceneViewCamera)
	{
		// TODO: render a texture of the scene camera framebuffer.
		// TODO: capture scene view input events when the scene view window is focused OR hovered.
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Camera"))
			{
				// Scene View Camera Settings
				ImGui::PushID("Scene View Camera");
				ImGuiUtil::DrawVector3Control("Position", sceneViewCamera.Position);
				ImGuiUtil::DrawCameraPropertiesControl("Properties", sceneViewCamera.Properties);
				ImGui::PopID();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void SceneEditorLayer::drawEditorWindowMenuBar(Scene &currentScene, std::filesystem::path &currentScenePathContext)
	{
		bool openLoadSceneFilePathPopup = false;
		bool openSaveSceneFilePathPopup = false;

		char const* loadScenePopupId = "Select a scene file (*.tscene)";
		char const* saveScenePopupId = "Save scene file as... ";

		if (ImGui::BeginMenuBar())
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
			if (ImGui::BeginMenu("Window"))
			{
				EditorWindowManager::ForEachEditorWindow(
					[](EditorWindow& editorWindow)
					{
						if (ImGui::MenuItem(editorWindow.Name, nullptr, editorWindow.IsConfigOpen))
						{
							editorWindow.IsConfigOpen = !editorWindow.IsConfigOpen;
						}
					});

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
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

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
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
	bool SceneEditorLayer::drawSceneSystemList(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate)
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

		// Draw a 'Add System' button at the top of the inspector, and align it to the right side of the window.
		char const* addSystemPopupId = "Add System Menu Popup";
		ImVec2 const addButtonSize = ImVec2 {120, 0};
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

		ImVec2 const addComponentButtonSize = ImVec2 {120, 0};
		float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY? ImGui::GetWindowScrollbarRect(ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;

		// Draw entity's NameComponent as a InputField on the top.
		auto& nameComponent = entity.AddOrGetComponent<NameComponent>();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() - scrollBarWidth - addComponentButtonSize.x - ImGui::GetFontSize());
		changed |= ImGui::InputText("##EntityNameComponent", &nameComponent.Name);
		ImGui::PopItemWidth();

		// Draw a 'Add Component' button at the top of the inspector, and align it to the right side of the window.
		char const* addComponentPopupId = "Add Component Menu Popup";
		float const availableWidthForAddButton = ImGui::GetWindowWidth() - scrollBarWidth;
		ImGui::SameLine();
		ImGui::SetCursorPosX(availableWidthForAddButton - addComponentButtonSize.x);
		if (ImGui::Button("Add Component", addComponentButtonSize))
		{
			ImGui::OpenPopup(addComponentPopupId);
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted("Add a new component to the entity.");
			ImGui::EndTooltip();
		}

		// Draw a separator between the header of the inspector and the component list.
		ImGui::Separator();

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
		bool isNameComponentSkipped = false;
		for (auto& [name, functions] : componentNamesAndFunctions)
		{
			if (!isNameComponentSkipped && name == NameComponentName)
			{
				// We don't want to draw name component as a normal component.
				// We've already drawn it on the top of the entity inspector.
				isNameComponentSkipped = true;
				continue;
			}

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

			ImGui::PushID(name.c_str());
			if (functions.DrawHeader == nullptr)
			{
				ImGuiTreeNodeFlags const flags = ImGuiTreeNodeFlags_DefaultOpen;
				showComponentInspector = ImGui::CollapsingHeader("##Header", &isHeaderVisible, flags);

				// Spacing ahead of the component name.
				float const spacing = ImGui::GetFrameHeight();
				ImGui::SameLine(); ImGui::ItemSize(ImVec2(spacing, 0));

				// The name of the component.
				ImGui::SameLine();
				ImGui::TextUnformatted(name.c_str());

			}
			else
			{
				// Use custom header drawer if provided.
				showComponentInspector = functions.DrawHeader(entity, isHeaderVisible, changed, name);
			}
			ImGui::PopID();

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

}