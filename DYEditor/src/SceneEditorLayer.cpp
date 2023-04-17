#include "SceneEditorLayer.h"

#include "SceneRuntimeLayer.h"
#include "Core/Application.h"
#include "Core/RuntimeState.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/WindowManager.h"
#include "Graphics/Framebuffer.h"
#include "Input/InputManager.h"
#include "Event/MouseEvent.h"
#include "Util/Time.h"
#include "EditorConfig.h"
#include "ImGui/EditorWindowManager.h"
#include "ImGui/EditorImGuiUtil.h"
#include "ImGui/ImGuiUtil.h"

#include "NameComponent.h"
#include "TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Systems/RegisterCameraSystem.h"

#include <filesystem>
#include <unordered_set>
#include <stack>
#include <iostream>
#include <imgui.h>
#include <imgui_stdlib.h>

using namespace DYE::DYEntity;

namespace DYE::DYEditor
{
	constexpr char const* k_DYEditorWindowId = "DYEditor";
	constexpr char const* k_DYEditorDockSpaceId = "DYEditor DockSpace";
	constexpr char const* k_SceneHierarchyWindowId = "Scene Hierarchy";
	constexpr char const* k_SceneSystemWindowId = "Scene System";
	constexpr char const* k_EntityInspectorWindowId = "Entity Inspector";
	constexpr char const* k_SceneViewWindowId = "Scene View";

	SceneEditorLayer::SceneEditorLayer() :
		LayerBase("Editor"),
		m_SerializedSceneCacheWhenEnterPlayMode(SerializedObjectFactory::CreateEmptySerializedScene())
	{
	}

	void SceneEditorLayer::OnAttach()
	{
		// DEBUGGING, Should be moved to DYEditorApplication so that both EditorLayer & RuntimeLayer could use it
		DYEditor::RegisterBuiltInTypes();
		DYEditor::RegisterUserTypes();

		// Load editor config file.
		EditorConfig::LoadFromOrCreateDefaultAt(EditorConfig::DefaultEditorConfigFilePath);

		// Register built-in editor windows.
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

		std::string const& defaultScenePath = EditorConfig::GetOrDefault<std::string>("Editor.DefaultScene", "");
		if (defaultScenePath.empty())
		{
			// Create an untitled new scene.

			// Add a camera entity & camera system by default if the active scene is untitled and empty.
			auto cameraEntity = m_RuntimeLayer->ActiveMainScene.World.CreateEntity("Camera");
			cameraEntity.AddComponent<TransformComponent>().Position = {0, 0, 10};
			cameraEntity.AddComponent<CameraComponent>();
			m_RuntimeLayer->ActiveMainScene.TryAddSystemByName(RegisterCameraSystem::TypeName);
		}
		else
		{
			// Load the default scene.
			std::optional<SerializedScene> serializedScene = SerializedObjectFactory::TryLoadSerializedSceneFromFile(defaultScenePath);
			if (serializedScene.has_value())
			{
				SerializedObjectFactory::ApplySerializedSceneToEmptyScene(serializedScene.value(), m_RuntimeLayer->ActiveMainScene);
				m_CurrentSceneFilePath = defaultScenePath;
			}
		}

		m_SceneViewCameraTargetFramebuffer = Framebuffer::Create(FramebufferProperties { .Width = 1600, .Height = 900 });
		m_SceneViewCamera.Properties.TargetType = RenderTargetType::RenderTexture;
		m_SceneViewCamera.Properties.pTargetRenderTexture = m_SceneViewCameraTargetFramebuffer.get();
		m_SceneViewCamera.Position = {0, 0, 10};

		// Register events.
		RuntimeState::RegisterListener(this);

		// FIXME: Remove temp secondary window setup
		auto windowPtr = WindowManager::CreateWindow(WindowProperties("Test Window", 640, 480));
		windowPtr->SetContext(WindowManager::GetMainWindow()->GetContext());
	}

	void SceneEditorLayer::OnDetach()
	{
		// Unregister events.
		RuntimeState::UnregisterListener(this);

		// Save current active scene as default scene for the next launch.
		EditorConfig::SetAndSave<std::string>("Editor.DefaultScene", m_CurrentSceneFilePath.string());

		EditorWindowManager::ClearRegisteredEditorWindows();
		TypeRegistry::ClearRegisteredComponentTypes();
		TypeRegistry::ClearRegisteredSystems();
	}

	void SceneEditorLayer::OnUpdate()
	{
		bool const isMiddleMouseButtonPressed = INPUT.GetMouseButton(MouseButton::Middle);

		if (isMiddleMouseButtonPressed && m_IsSceneViewWindowHovered)
		{
			// Focus SceneView window if middle mouse button is pressed over the SceneView window.
			ImGui::SetWindowFocus(k_SceneViewWindowId);
		}

		if (!m_IsSceneViewWindowFocused)
		{
			// Skip SceneView camera input if the SceneView window is not focused.
			return;
		}

		// Scene View Camera Input.
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

		if (isMiddleMouseButtonPressed)
		{
			// TODO: the delta should be scaled based on the dimensions of the scene view window.
			//		So the camera panning speed matches up with the mouse delta.
			auto mouseDelta = INPUT.GetGlobalMouseDelta();

			glm::vec2 panMove = mouseDelta;
			panMove *= m_CameraMousePanMoveUnitPerSecond * TIME.DeltaTime();
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
			if (!m_IsSceneViewWindowHovered)
			{
				return;
			}

			// When SceneView window hovered, zooming should always work no matter focused or not.
			auto mouseScrolledEvent = (MouseScrolledEvent&) event;
			m_SceneViewCamera.Properties.OrthographicSize -= TIME.DeltaTime() * m_CameraOrthographicSizeZoomSpeedMultiplier * mouseScrolledEvent.GetY();
			if (m_SceneViewCamera.Properties.OrthographicSize < 0.1f)
			{
				m_SceneViewCamera.Properties.OrthographicSize = 0.1f;
			}

			// Use the event up.
			mouseScrolledEvent.IsUsed = true;
		}
	}

	void SceneEditorLayer::OnRender()
	{
		DYE::RenderPipelineManager::RegisterCameraForNextRender(m_SceneViewCamera);
	}

	void SceneEditorLayer::OnPlayModeStateChanged(DYE::DYEditor::PlayModeStateChange stateChange)
	{
		if (stateChange == PlayModeStateChange::BeforeEnterPlayMode)
		{
			// Initialize systems.
			auto &scene = m_RuntimeLayer->ActiveMainScene;
			scene.ForEachSystemDescriptor
			(
				[&scene](SystemDescriptor &systemDescriptor, ExecutionPhase phase)
				{
					systemDescriptor.Instance->InitializeLoad(
						scene.World,
						InitializeLoadParameters
						{
							.LoadType = InitializeLoadType::BeforeEnterPlayMode
						});
				}
			);

			// Save a copy of the active scene as a serialized scene.
			m_SerializedSceneCacheWhenEnterPlayMode = SerializedObjectFactory::CreateSerializedScene(scene);
		}
		else if (stateChange == PlayModeStateChange::BeforeEnterEditMode)
		{
			// Initialize systems.
			auto &scene = m_RuntimeLayer->ActiveMainScene;
			scene.ForEachSystemDescriptor
			(
				[&scene](SystemDescriptor &systemDescriptor, ExecutionPhase phase)
				{
					systemDescriptor.Instance->InitializeLoad(
						scene.World,
						InitializeLoadParameters
						{
							.LoadType = InitializeLoadType::BeforeEnterEditMode
						});
				}
			);

			// Reapply the serialized scene back to the active scene.
			// TODO: have an option to keep the changes in play mode.
			scene.Clear();
			SerializedObjectFactory::ApplySerializedSceneToEmptyScene(m_SerializedSceneCacheWhenEnterPlayMode, scene);
		}
	}

	void SceneEditorLayer::OnImGui()
	{
		DYE_ASSERT_LOG_WARN(m_RuntimeLayer, "SceneRuntimeLayer is null. You might have forgot to call SetRuntimeLayer.");

		Scene &activeScene = m_RuntimeLayer->ActiveMainScene;

		// Create the main editor window that can be docked by other editor windows.
		// By default, it also hosts all the major windows such as Scene Hierarchy Panel, Entity Inspector & Scene View etc.
		ImGuiWindowFlags mainEditorWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGuiDockNodeFlags mainEditorWindowDockSpaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		float const editorLayerWindowPadding = 10;
		ImVec2 const editorLayerWindowPos = { viewport->WorkPos.x + editorLayerWindowPadding, viewport->WorkPos.y };
		ImVec2 const editorLayerWindowSize = { viewport->WorkSize.x - editorLayerWindowPadding * 2, viewport->WorkSize.y - editorLayerWindowPadding };
		ImGui::SetNextWindowPos(editorLayerWindowPos, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(editorLayerWindowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(0.35f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(k_DYEditorWindowId, nullptr, mainEditorWindowFlags);
		ImGui::PopStyleVar();

		drawEditorWindowMenuBar(activeScene, m_CurrentSceneFilePath);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockSpaceId = ImGui::GetID(k_DYEditorDockSpaceId);
			if (ImGui::DockBuilderGetNode(dockSpaceId) == nullptr)
			{
				setEditorWindowDefaultLayout(dockSpaceId);
			}
			ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), mainEditorWindowDockSpaceFlags);
		}

		ImGuiViewport const *mainEditorWindowViewport = ImGui::GetWindowViewport();
		ImGui::End();

		// Draw all the major editor windows.
		// TODO: right now we make all the major windows transparent (alpha = 0.35f)
		//		We might want to make it configurable for the users & having two values for Edit Mode and Play Mode
		ImGuiWindowFlags const sceneViewWindowFlags = ImGuiWindowFlags_MenuBar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0, 0});
		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin(k_SceneViewWindowId, nullptr, sceneViewWindowFlags))
		{
			m_IsSceneViewWindowFocused = ImGui::IsWindowFocused();
			m_IsSceneViewWindowHovered = ImGui::IsWindowHovered();

			// ImGuiLayer shouldn't block events when SceneView window is focused OR hovered.
			bool const editorShouldReceiveCameraInputEvent = m_IsSceneViewWindowFocused || m_IsSceneViewWindowHovered;
			m_pApplication->GetImGuiLayer().SetBlockEvents(!editorShouldReceiveCameraInputEvent);

			drawSceneView(m_SceneViewCamera);
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin(k_SceneSystemWindowId))
		{
			drawSceneSystemPanel(activeScene);
		}
		ImGui::End();

		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin(k_SceneHierarchyWindowId))
		{
			drawSceneEntityHierarchyPanel(activeScene, &m_CurrentlySelectedEntityInHierarchyPanel);
		}
		ImGui::End();

		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin(k_EntityInspectorWindowId))
		{
			drawEntityInspector(m_CurrentlySelectedEntityInHierarchyPanel, TypeRegistry::GetComponentTypesNamesAndFunctionCollections());
		}
		ImGui::End();

		// Draw other generic editor windows.
		EditorWindowManager::DrawEditorWindows(mainEditorWindowViewport);
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

					auto cameraEntity = currentScene.World.CreateEntity("Camera");
					cameraEntity.AddComponent<TransformComponent>().Position = {0, 0, 10};
					cameraEntity.AddComponent<CameraComponent>();
					currentScene.TryAddSystemByName(RegisterCameraSystem::TypeName);
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

				if (ImGui::MenuItem("Save Scene As..."))
				{
					// We store a flag here and delay opening the popup
					// because MenuItem is Selectable and Selectable by default calls CloseCurrentPopup().
					openSaveSceneFilePathPopup = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Play", "Ctrl+P", RuntimeState::IsPlaying()))
				{
					RuntimeState::SetIsPlaying(!RuntimeState::IsPlaying());
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::BeginMenu("Layouts"))
				{
					if (ImGui::MenuItem("Default"))
					{
						DYE_MSG_BOX(SDL_MESSAGEBOX_WARNING, "NOT IMPLEMENTED FEATURE",
									"Default Layout is not properly implemented.\n"
									"You can delete imgui.ini file to reset the layout manually.");
						//setEditorWindowDefaultLayout(ImGui::GetID(k_DYEditorDockSpaceId));
					}

					ImGui::Separator();

					bool const attachDYEditorToWindow = ImGui::MenuItem("Attach DYEditor");
					ImGui::SameLine();
					ImGuiUtil::DrawHelpMarker("Attach DYEditor to Main Viewport at the default position with the default size.");
					if (attachDYEditorToWindow)
					{
						const ImGuiViewport *viewport = ImGui::GetMainViewport();
						float const editorLayerWindowPadding = 10;
						ImVec2 const editorLayerWindowPos = {viewport->WorkPos.x + editorLayerWindowPadding,
															 viewport->WorkPos.y};
						ImVec2 const editorLayerWindowSize = {viewport->WorkSize.x - editorLayerWindowPadding * 2,
															  viewport->WorkSize.y - editorLayerWindowPadding};
						ImGui::SetWindowPos(k_DYEditorWindowId, editorLayerWindowPos);
						ImGui::SetWindowSize(k_DYEditorWindowId, editorLayerWindowSize);
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

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

	void SceneEditorLayer::drawSceneView(Camera &sceneViewCamera)
	{
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

		// Render SceneView as a texture image.
		ImVec2 const sceneViewWindowSize = ImGui::GetContentRegionAvail();

		if (sceneViewWindowSize.x <= 0 || sceneViewWindowSize.y <= 0)
		{
			// If either width or height is 0, we don't need to draw the texture OR resize the framebuffer.
			// Because the scene view window is very likely folded.
			return;
		}

		auto const& renderTextureProperties = sceneViewCamera.Properties.pTargetRenderTexture->GetProperties();
		if (renderTextureProperties.Width != sceneViewWindowSize.x || renderTextureProperties.Height != sceneViewWindowSize.y)
		{
			// Resize the target framebuffer (render texture) if the window avail region size is different with the framebuffer size.
			sceneViewCamera.Properties.pTargetRenderTexture->Resize(sceneViewWindowSize.x, sceneViewWindowSize.y);
		}

		auto sceneViewRenderTextureID = sceneViewCamera.Properties.pTargetRenderTexture->GetColorAttachmentID();
		auto imTexID = (void*)(intptr_t)(sceneViewRenderTextureID);
		ImVec2 const uv0 = ImVec2(0, 1); ImVec2 const uv1 = ImVec2(1, 0);
		ImGui::Image(imTexID, sceneViewWindowSize, uv0, uv1);
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

	bool SceneEditorLayer::drawSceneSystemPanel(Scene& scene)
	{
		bool changed = false;

		#pragma unroll
		for (int phaseIndex = static_cast<int>(ExecutionPhase::Initialize);
			 phaseIndex <= static_cast<int>(ExecutionPhase::TearDown); phaseIndex++)
		{
			auto const phase = static_cast<ExecutionPhase>(phaseIndex);
			bool const isPhaseRunning = RuntimeState::IsPlaying() || (phase == ExecutionPhase::Render || phase == ExecutionPhase::PostRender);

			std::string const &phaseId = CastExecutionPhaseToString(phase);
			auto& systemDescriptors = scene.GetSystemDescriptorsOfPhase(phase);

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
				changed |= drawSceneSystemList(scene, systemDescriptors,
									[phase](std::string const &systemName, SystemBase const *pInstance)
									{
										return pInstance->GetPhase() == phase;
									});
			}
			ImGui::PopID();
		}

		ImGui::Separator();
		if (!scene.UnrecognizedSystems.empty())
		{
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.000f, 0.000f, 0.000f, 0.310f));
			bool const showUnrecognizedSystems = ImGui::CollapsingHeader("Unrecognized Systems");
			ImGui::PopStyleColor();
			if (showUnrecognizedSystems)
			{
				if (ImGui::BeginTable("Unrecognized System Table", 1, ImGuiTableFlags_RowBg))
				{
					int indexToRemove = -1;
					for (int i = 0; i < scene.UnrecognizedSystems.size(); ++i)
					{
						auto const &descriptor = scene.UnrecognizedSystems[i];

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
						scene.UnrecognizedSystems.erase(scene.UnrecognizedSystems.begin() + indexToRemove);
						changed = true;
					}

					ImGui::EndTable();
				}
			}
		}

		return changed;
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