#include "SceneEditorLayer.h"

#include "SceneRuntimeLayer.h"
#include "Core/Application.h"
#include "Core/RuntimeState.h"
#include "Core/RuntimeSceneManagement.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/WindowManager.h"
#include "Graphics/Framebuffer.h"
#include "Input/InputManager.h"
#include "Event/MouseEvent.h"
#include "Core/Time.h"
#include "Util/EntityUtil.h"
#include "ProjectConfig.h"
#include "ImGui/EditorWindowManager.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/ImGuiUtil_Internal.h"
#include "Undo/Undo.h"

#include "Components/NameComponent.h"
#include "Components/HierarchyComponents.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Systems/RegisterCameraSystem.h"

#include <filesystem>
#include <unordered_set>
#include <stack>
#include <iostream>

#include <imgui.h>
#include <imgui_stdlib.h>

using namespace DYE::DYEditor;

namespace DYE::DYEditor
{
	constexpr char const* k_DYEditorWindowId = "###DYEditor";
	constexpr char const* k_DYEditorDockSpaceId = "DYEditor DockSpace";
	constexpr char const* k_SceneHierarchyWindowId = "Scene Hierarchy";
	constexpr char const* k_SceneSystemWindowId = "Scene System";
	constexpr char const* k_EntityInspectorWindowId = "###EntityInspector";
	constexpr char const* k_SceneViewWindowId = "Scene View";

	SceneEditorLayer::SceneEditorLayer() :
		LayerBase("Editor"),
		m_SerializedSceneCacheWhenEnterPlayMode(SerializedObjectFactory::CreateEmptySerializedScene())
	{
	}

	void SceneEditorLayer::OnAttach()
	{
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

		EditorWindowManager::RegisterEditorWindow(
			RegisterEditorWindowParameters
				{
					.Name = "Editor Configuration",
					.isConfigOpenByDefault = false
				},
			[](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
			{
				GetEditorConfig().DrawGenericConfigurationBrowserImGui("Editor Configuration", pIsOpen);
			}
		);

		EditorWindowManager::RegisterEditorWindow(
			RegisterEditorWindowParameters
				{
					.Name = "Runtime Configuration",
					.isConfigOpenByDefault = false
				},
			[](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
			{
				DrawRuntimeConfigurationWindow(pIsOpen);
			}
		);

		EditorWindowManager::RegisterEditorWindow(
			RegisterEditorWindowParameters
				{
					.Name = "Undo History",
					.isConfigOpenByDefault = false
				},
			[](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
			{
				Undo::DrawUndoHistoryWindow(pIsOpen);
			}
		);

		// Load the default scene indicated in editor config file.
		std::string const &defaultScenePath = GetEditorConfig().GetOrDefault<std::string>("Editor.DefaultScene", "");
		std::optional<SerializedScene> serializedScene = SerializedObjectFactory::TryLoadSerializedSceneFromFile(defaultScenePath);
		if (serializedScene.has_value())
		{
			SerializedObjectFactory::ApplySerializedSceneToEmptyScene(serializedScene.value(), RuntimeSceneManagement::GetActiveMainScene());
			m_CurrentSceneFilePath = defaultScenePath;
		}
		else
		{
			// Failed to load the default scene, therefore we create an untitled new scene.
			// Add a camera entity & camera system by default if the active scene is untitled and empty.
			auto cameraEntity = RuntimeSceneManagement::GetActiveMainScene().World.CreateEntity("Camera");
			cameraEntity.AddComponent<TransformComponent>().Position = {0, 0, 10};
			cameraEntity.AddComponent<CameraComponent>();
			RuntimeSceneManagement::GetActiveMainScene().TryAddSystemByName(RegisterCameraSystem::TypeName);
		}

		// Register events.
		RuntimeState::RegisterListener(this);

		// Initialize SceneView framebuffer.
		m_SceneViewCameraTargetFramebuffer = Framebuffer::Create(FramebufferProperties {.Width = 1600, .Height = 900});
		m_SceneViewCameraTargetFramebuffer->SetDebugLabel("Scene View Framebuffer");
		m_SceneViewCamera.Properties.TargetType = RenderTargetType::RenderTexture;
		m_SceneViewCamera.Properties.pTargetRenderTexture = m_SceneViewCameraTargetFramebuffer.get();
		m_SceneViewCamera.Position = {0, 0, 10};

		// Do some more editor setup based on EditorConfig settings.
		m_InspectorMode = GetEditorConfig().GetOrDefault("Editor.DebugInspector", false)? InspectorMode::Debug : InspectorMode::Normal;

		// FIXME: Window setup should be made according to runtime config file (i.e. GetRuntimeConfig().GetOrDefault("Window.NumberOfInitialWindows")...).
		//		For now we create a secondary window manually.
		auto windowPtr = WindowManager::CreateWindow(WindowProperties("Test Window", 640, 480));
		windowPtr->SetContext(WindowManager::GetMainWindow()->GetContext());
	}

	void SceneEditorLayer::OnDetach()
	{
		// Unregister events.
		RuntimeState::UnregisterListener(this);

		// Save current active scene as default scene for the next launch.
		GetEditorConfig().SetAndSave<std::string>("Editor.DefaultScene", m_CurrentSceneFilePath.string());

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

	void SceneEditorLayer::OnPlayModeStateChanged(DYE::DYEditor::ModeStateChange stateChange)
	{
		auto &scene = RuntimeSceneManagement::GetActiveMainScene();
		if (stateChange == ModeStateChange::BeforeEnterPlayMode)
		{
			// Save a copy of the active scene as a serialized scene.
			m_SerializedSceneCacheWhenEnterPlayMode = SerializedObjectFactory::CreateSerializedScene(scene);

			// Initialize load systems.
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

			// Execute initialize systems.
			scene.ExecuteInitializeSystems();
		}
		else if (stateChange == ModeStateChange::BeforeEnterEditMode)
		{
			// Execute teardown systems.
			scene.ExecuteTeardownSystems();

			// Initialize load systems.
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
			// TODO: maybe have an option to keep the changes in play mode?
			scene.Clear();
			SerializedObjectFactory::ApplySerializedSceneToEmptyScene(m_SerializedSceneCacheWhenEnterPlayMode, scene);
		}
	}

	void SceneEditorLayer::OnImGui()
	{
		DYE_ASSERT_LOG_WARN(m_RuntimeLayer, "SceneRuntimeLayer is null. You might have forgot to call SetRuntimeLayer.");

		Scene &activeScene = RuntimeSceneManagement::GetActiveMainScene();

		// Create the main editor window that can be docked by other editor windows.
		// By default, it also hosts all the major windows such as Scene Hierarchy Panel, Entity Inspector & Scene View etc.
		ImGuiWindowFlags mainEditorWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGuiDockNodeFlags mainEditorWindowDockSpaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

		if (m_IsActiveSceneDirty)
		{
			mainEditorWindowFlags |= ImGuiWindowFlags_UnsavedDocument;
		}

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		float const editorLayerWindowPadding = 10;
		ImVec2 const editorLayerWindowPos = { viewport->WorkPos.x + editorLayerWindowPadding, viewport->WorkPos.y };
		ImVec2 const editorLayerWindowSize = { viewport->WorkSize.x - editorLayerWindowPadding * 2, viewport->WorkSize.y - editorLayerWindowPadding };
		ImGui::SetNextWindowPos(editorLayerWindowPos, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(editorLayerWindowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(0.35f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		char mainEditorName[32];
		sprintf(mainEditorName, "%s%s", RuntimeState::IsPlaying()? "DYEditor (Play Mode)" : "DYEditor (Edit Mode)", k_DYEditorWindowId);
		ImGui::Begin(mainEditorName, nullptr, mainEditorWindowFlags);
		ImGui::PopStyleVar();

		drawEditorWindowMenuBar(activeScene, m_CurrentSceneFilePath, &m_IsActiveSceneDirty);

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

		// Draw generic editor windows.
		// We call this before drawing major editor windows because Undo History could possibly delete entity with texture reference
		// That is being drawn inside entity inspector.
		// If we release a texture id that has already been submitted to the imgui drawlist, the program will be asserted.
		EditorWindowManager::DrawEditorWindows(mainEditorWindowViewport);

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
			bool const isSystemListModified = drawSceneSystemPanel(activeScene);
			m_IsActiveSceneDirty |= isSystemListModified;
		}
		ImGui::End();

		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGuiWindowFlags const hierarchyWindowFlags = m_IsActiveSceneDirty? ImGuiWindowFlags_UnsavedDocument : ImGuiWindowFlags_None;
		if (ImGui::Begin(k_SceneHierarchyWindowId, nullptr, hierarchyWindowFlags))
		{
			bool const isHierarchyChanged = drawSceneEntityHierarchyPanel(activeScene, &m_CurrentlySelectedEntityInHierarchyPanel);
			m_IsActiveSceneDirty |= isHierarchyChanged;
		}
		ImGui::End();

		ImGui::SetNextWindowBgAlpha(0.35f);

		// We want to draw window with different titles in different mode (normal/debug).
		char entityInspectorWindowName[128];
		bool const debugMode = m_InspectorContext.Mode == InspectorMode::Debug;
		sprintf(entityInspectorWindowName, "%s%s", debugMode ? "Entity Inspector (Debug)" : "Entity Inspector", k_EntityInspectorWindowId);
		if (ImGui::Begin(entityInspectorWindowName))
		{
			// Draw inspector window title context menu.
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Normal", nullptr, !debugMode))
				{
					m_InspectorContext.Mode = InspectorMode::Normal;
					GetEditorConfig().SetAndSave("Editor.DebugInspector", false);
				}

				if (ImGui::MenuItem("Debug", nullptr, debugMode))
				{
					m_InspectorContext.Mode = InspectorMode::Debug;
					GetEditorConfig().SetAndSave("Editor.DebugInspector", true);
				}

				ImGui::EndPopup();
			}

			m_InspectorContext.Entity = m_CurrentlySelectedEntityInHierarchyPanel;
			bool const isEntityChanged = drawEntityInspector(m_InspectorContext, TypeRegistry::GetComponentTypesNamesAndDescriptors());
			m_IsActiveSceneDirty |= isEntityChanged;
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

	void SceneEditorLayer::drawEditorWindowMenuBar(Scene &currentScene, std::filesystem::path &currentScenePathContext,
												   bool *pIsSceneDirty)
	{
		bool openLoadSceneFilePathPopup = false;
		bool openSaveSceneFilePathPopup = false;

		char const* loadScenePopupId = "Select a scene file (*.tscene)";
		char const* saveScenePopupId = "Save scene file as... ";

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", nullptr, false, !RuntimeState::IsPlaying()))
				{
					currentScenePathContext.clear();
					currentScene.Clear();

					auto cameraEntity = currentScene.World.CreateEntity("Camera");
					cameraEntity.AddComponent<TransformComponent>().Position = {0, 0, 10};
					cameraEntity.AddComponent<CameraComponent>();
					currentScene.TryAddSystemByName(RegisterCameraSystem::TypeName);

					Undo::ClearAll();
					*pIsSceneDirty = false;
				}

				if (ImGui::MenuItem("Open Scene", nullptr, false, !RuntimeState::IsPlaying()))
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
						*pIsSceneDirty = false;
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
				if (ImGui::MenuItem("Undo", "Ctrl+Z", false, Undo::HasOperationToUndo()))
				{
					Undo::PerformUndo();
				}

				if (ImGui::MenuItem("Redo", "Ctrl+Y", false, Undo::HasOperationToRedo()))
				{
					Undo::PerformRedo();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Play", "Ctrl+P", RuntimeState::IsPlaying()))
				{
					RuntimeState::SetIsPlayingAtTheEndOfFrame(!RuntimeState::IsPlaying());
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
			Undo::ClearAll();
			*pIsSceneDirty = false;
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
			*pIsSceneDirty = false;
		}
	}

	void SceneEditorLayer::drawSceneView(Camera &sceneViewCamera)
	{
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
		bool changed = false;

		// Draw scene hierarchy context menu.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Create Empty"))
			{
				// Select the newly created entity.
				// For now, the entity is always put at the end of the list.
				*pCurrentSelectedEntity = scene.World.CreateEntity("Entity");
				int const indexInWorldArray = scene.World.GetNumberOfEntities() - 1;
				Undo::RegisterEntityCreation(scene.World, *pCurrentSelectedEntity, indexInWorldArray);
				changed = true;
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

		struct DebugContext
		{
			bool ShowWidgetRect = false;
			bool ShowTreeNodeRect = false;
		};
		static DebugContext debugContext;

		struct MoveEntity
		{
			bool HasOperation = false;
			Entity Entity;
			std::size_t SrcIndex = 0;
			std::size_t DstIndex = 0;

			bool BecomeChildOfDst = false;
		};
		MoveEntity moveEntity;

		auto itemSpacing = ImGui::GetStyle().ItemSpacing; itemSpacing.y = 6;
		auto framePadding = ImGui::GetStyle().FramePadding; framePadding.x = 3;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, framePadding);
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 13);

		struct HierarchyLevel
		{
			DYE::GUID LevelParentGUID = (DYE::GUID) 0;
			std::size_t NumberOfEntitiesLeft = 0;
			bool IsOpen = false;
		};
		std::vector<HierarchyLevel> levelStack;
		// We reserve to make sure the array doesn't re-allocate during foreach.
		levelStack.reserve(scene.World.GetNumberOfEntities());

		// Draw all entities.
		scene.World.ForEachEntityAndIndex
		(
			[&changed,
			 &scene,
			 &pCurrentSelectedEntity,
			 &levelStack,
			 &moveEntity]
			 (DYEditor::Entity &entity, std::size_t indexInWorld)
			{
				auto tryGetNameResult = entity.TryGetName();
				if (!tryGetNameResult.has_value())
				{
					// No name, skip it.
					return;
				}

				auto tryGetGUIDResult = entity.TryGetGUID();
				if (!tryGetGUIDResult.has_value())
				{
					// No GUID, skip it.
					return;
				}

				auto &name = tryGetNameResult.value();
				auto &guid = tryGetGUIDResult.value();
				auto tryGetChildrenComponent = entity.TryGetComponent<ChildrenComponent>();

				auto tryGetParent = entity.TryGetComponent<ParentComponent>();
				auto parentGUID = tryGetParent.has_value()? tryGetParent.value().get().ParentGUID : (DYE::GUID) 0;

				std::size_t childrenCount = tryGetChildrenComponent.has_value()? tryGetChildrenComponent.value().get().ChildrenGUIDs.size() : 0;

				HierarchyLevel *pLevel = levelStack.empty()? nullptr : &levelStack.back();
				while
				(
					pLevel != nullptr &&
					pLevel->LevelParentGUID != parentGUID &&
					pLevel->NumberOfEntitiesLeft == 0
				)
				{
					// The top level is not the parent of the current entity
					// && It's also an empty level (all the entities of that level have been iterated)
					// -> We will pop the top level.

					// If the tree node of the level is open,
					// we need to pop it.
					if (pLevel->IsOpen)
					{
						ImGui::TreePop();
					}

					// Pop the level from the stack.
					levelStack.pop_back();

					pLevel = levelStack.empty()? nullptr : &levelStack.back();
				}

				bool const isEntityShown = pLevel == nullptr || pLevel->IsOpen;
				bool const isLeafNode = childrenCount == 0;

				bool const isSelected = entity == *pCurrentSelectedEntity;

				ImVec2 const entityTreeNodeScreenPos = ImGui::GetCursorScreenPos();
				ImVec2 const entityWidgetSize = ImVec2(ImGui::GetContentRegionAvail().x,
													   ImGui::GetTextLineHeightWithSpacing());

				float const spacingBetweenTreeNode = ImGui::GetStyle().ItemSpacing.y;
				ImVec2 const entityWidgetScreenPos = ImVec2(entityTreeNodeScreenPos.x, entityTreeNodeScreenPos.y - spacingBetweenTreeNode * 0.5f);

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
				if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
				if (isLeafNode)
				{
					flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				}

				bool isNodeOpen = false;
				if (isEntityShown)
				{
					// We use GUID as the id of the TreeNode.
					isNodeOpen = ImGui::TreeNodeEx(guid.ToString().c_str(), flags, name.c_str()) && !isLeafNode;
					if (ImGui::BeginPopupContextItem())
					{
						// Draw entity context menu right after TreeNode call.
						if (ImGui::Selectable("Delete"))
						{
							Undo::DeleteEntity(scene.World, entity, indexInWorld);
							changed = true;
						}
						if (ImGui::Selectable("Create Empty"))
						{
							// Select the newly created entity.
							// For now, the entity is always put at the end of the list.
							// TODO: make it a child of the clicked tree node entity.
							*pCurrentSelectedEntity = scene.World.CreateEntity("Entity");
							int const indexInWorldArray = scene.World.GetNumberOfEntities() - 1;
							Undo::RegisterEntityCreation(scene.World, *pCurrentSelectedEntity, indexInWorldArray);
							changed = true;
						}
						ImGui::EndPopup();
					}
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						*pCurrentSelectedEntity = entity;
					}

					// FIXME: For debugging
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted("Children with preorder:");
						EntityUtil::ForEachChildRecursive
						(
							entity,
							[](Entity childEntity)
							{
								  ImGui::TextUnformatted(childEntity.TryGetName().value().c_str());
							}
						);
						ImGui::EndTooltip();
					}

					ImVec2 const entityTreeNodeSize = ImGui::GetItemRectSize();
					float const entityWidgetCenterY = entityTreeNodeScreenPos.y + entityTreeNodeSize.y * 0.5f;

					if (debugContext.ShowTreeNodeRect || debugContext.ShowWidgetRect)
					{
						ImVec2 const widgetHeadCenter = ImVec2(entityTreeNodeScreenPos.x, entityWidgetCenterY);
						ImGui::GetWindowDrawList()->AddCircle(widgetHeadCenter, 2, ImColor(ImVec4(1, 1, 1, 0.5f)), 4);
					}

					if (debugContext.ShowTreeNodeRect)
					{
						ImGui::GetWindowDrawList()->AddCircle(entityTreeNodeScreenPos, 2, ImColor(ImVec4(1, 1, 1, 1)), 4);
						ImGui::GetWindowDrawList()->AddRect(entityTreeNodeScreenPos,
															ImVec2(entityTreeNodeScreenPos.x + entityTreeNodeSize.x,
																   entityTreeNodeScreenPos.y + entityTreeNodeSize.y),
															ImColor(ImVec4(1, 1, 1, 0.5f)));
					}

					if (debugContext.ShowWidgetRect)
					{
						ImGui::GetWindowDrawList()->AddCircle(entityWidgetScreenPos, 2, ImColor(ImVec4(0, 1, 0, 1)), 4);
						ImGui::GetWindowDrawList()->AddRect(entityWidgetScreenPos,
															ImVec2(entityWidgetScreenPos.x + entityWidgetSize.x,
																   entityWidgetScreenPos.y + entityWidgetSize.y),
															ImColor(ImVec4(0, 1, 0, 0.5f)));
					}

					// Make the entity tree node a drag source.
					ImGuiDragDropFlags const dragHandleFlags = ImGuiDragDropFlags_None;
					if (ImGui::BeginDragDropSource(dragHandleFlags))
					{
						ImGui::SetDragDropPayload("EntityIndex", &indexInWorld, sizeof(std::size_t));

						// Preview the entity in the drag tooltip.
						ImGui::Text(name.c_str());

						ImGui::EndDragDropSource();
					}

					// We draw invisible drop handles if the user is dragging an entity.
					if (ImGui::IsDragDropActive())
					{
						ImVec2 const originalCursorPos = ImGui::GetCursorPos();

						// Divide the entity widget into 3 drop handle rows:
						// 	1. Upper: insert the dragged entity above the dropped entity.
						//  2. Middle: make the dragged entity a child of the dropped entity.
						//  3. Lower: insert the dragged entity below the dropped entity.
						float const upperSize = spacingBetweenTreeNode * 0.5f;
						float const lowerSize = spacingBetweenTreeNode * 0.5f;
						float const middleSize = entityWidgetSize.y - upperSize - lowerSize;

						ImGui::SetCursorScreenPos(entityWidgetScreenPos);
						ImGuiUtil::Internal::InteractableItem("EntityDropHandle_Upper",
															  ImVec2(entityWidgetSize.x, upperSize));
						if (ImGui::BeginDragDropTarget())
						{
							ImGuiPayload const *dropPayload = ImGui::AcceptDragDropPayload("EntityIndex",
																						   ImGuiDragDropFlags_AcceptPeekOnly);
							if (dropPayload != nullptr)
							{
								DYE_ASSERT(dropPayload->DataSize == sizeof(std::size_t));
								std::size_t payloadIndex = *(const std::size_t *) dropPayload->Data;
								bool const isSource = payloadIndex == indexInWorld;

								if (dropPayload->IsPreview())
								{
									ImVec2 const previewLineBegin = entityWidgetScreenPos;
									ImVec2 const previewLineEnd = ImVec2(previewLineBegin.x + entityWidgetSize.x,
																		 previewLineBegin.y);
									ImGui::GetWindowDrawList()->AddLine(previewLineBegin, previewLineEnd,
																		ImGui::GetColorU32(ImGuiCol_DragDropTarget), 2);
								}

								if (dropPayload->IsDelivery() && !isSource)
								{
									moveEntity.HasOperation = true;
									moveEntity.SrcIndex = payloadIndex;
									moveEntity.DstIndex = indexInWorld;

									DYE_LOG("DROP UPPER");
								}
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SetCursorScreenPos(
							ImVec2(entityWidgetScreenPos.x, entityWidgetScreenPos.y + upperSize));
						ImGuiUtil::Internal::InteractableItem("EntityDropHandle_Middle",
															  ImVec2(entityWidgetSize.x, middleSize));
						if (ImGui::BeginDragDropTarget())
						{
							ImGuiPayload const *dropPayload = ImGui::AcceptDragDropPayload("EntityIndex",
																						   ImGuiDragDropFlags_AcceptBeforeDelivery);
							if (dropPayload != nullptr)
							{
								DYE_ASSERT(dropPayload->DataSize == sizeof(std::size_t));
								std::size_t payloadIndex = *(const std::size_t *) dropPayload->Data;
								bool const isSource = payloadIndex == indexInWorld;

								if (dropPayload->IsDelivery() && !isSource)
								{
									moveEntity.HasOperation = true;
									moveEntity.SrcIndex = payloadIndex;
									moveEntity.DstIndex = indexInWorld;
									moveEntity.BecomeChildOfDst = true;

									DYE_LOG("DROP MIDDLE BEFORE");

									// Open/expand the dropped entity (new parent) tree node.
									ImGuiID newParentTreeNodeId = ImGui::GetCurrentWindow()->GetID(guid.ToString().c_str());
									ImGui::TreeNodeSetOpen(newParentTreeNodeId, true);

									DYE_LOG("DROP MIDDLE AFTER");
								}
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SetCursorScreenPos(
							ImVec2(entityWidgetScreenPos.x, entityWidgetScreenPos.y + upperSize + middleSize));
						ImGuiUtil::Internal::InteractableItem("EntityDropHandle_Lower",
															  ImVec2(entityWidgetSize.x, lowerSize));
						if (ImGui::BeginDragDropTarget())
						{
							ImGuiPayload const *dropPayload = ImGui::AcceptDragDropPayload("EntityIndex",
																						   ImGuiDragDropFlags_AcceptPeekOnly);
							if (dropPayload != nullptr)
							{
								DYE_ASSERT(dropPayload->DataSize == sizeof(std::size_t));
								std::size_t payloadIndex = *(const std::size_t *) dropPayload->Data;
								bool const isSource = payloadIndex == indexInWorld;

								if (dropPayload->IsPreview())
								{
									ImVec2 const previewLineBegin = ImVec2(entityWidgetScreenPos.x,
																		   entityWidgetScreenPos.y + entityWidgetSize.y);
									ImVec2 const previewLineEnd = ImVec2(previewLineBegin.x + entityWidgetSize.x,
																		 previewLineBegin.y);
									ImGui::GetWindowDrawList()->AddLine(previewLineBegin, previewLineEnd,
																		ImGui::GetColorU32(ImGuiCol_DragDropTarget), 2);
								}

								if (dropPayload->IsDelivery() && !isSource)
								{
									moveEntity.HasOperation = true;
									moveEntity.SrcIndex = payloadIndex;
									moveEntity.DstIndex = indexInWorld + 1;

									DYE_LOG("DROP LOWER");
								}
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SetCursorPos(originalCursorPos);
					}

				}

				if (pLevel != nullptr)
				{
					pLevel->NumberOfEntitiesLeft--;
				}

				if (childrenCount > 0)
				{
					levelStack.push_back
					(
						{
							.LevelParentGUID = guid,
							.NumberOfEntitiesLeft = childrenCount,
							.IsOpen = isNodeOpen
						}
					);
				}
			}
		);

		for (auto level : levelStack)
		{
			// There are some levels left in the stack,
			// we need to make sure to pop the corresponding tree nodes if they are open.
			if (!level.IsOpen)
			{
				continue;
			}

			ImGui::TreePop();
		}

		// Pop ItemSpacing, FramePadding and IndentSpacing.
		ImGui::PopStyleVar(3);

		int moveDiff = ((int) moveEntity.DstIndex - (int) moveEntity.SrcIndex);

		if (moveEntity.HasOperation)
		{
			if (moveEntity.BecomeChildOfDst)
			{
				Entity srcEntity = scene.World.GetEntityAtIndex(moveEntity.SrcIndex);
				Entity dstEntity = scene.World.GetEntityAtIndex(moveEntity.DstIndex);

				Undo::SetEntityParent
				(
					srcEntity,
					moveEntity.SrcIndex,
					dstEntity,
					moveEntity.DstIndex
				);
			}
			else if (moveDiff != 1 && moveDiff != 0)
			{
				Undo::MoveEntity(scene.World.GetEntityAtIndex(moveEntity.SrcIndex), moveEntity.SrcIndex,
								 moveEntity.DstIndex);
				changed = true;
			}
		}

		// FIXME: delete debugging drawing here
		ImGui::Separator();
		ImGui::Checkbox("Show Widget Rect", &debugContext.ShowWidgetRect);
		ImGui::Checkbox("Show TreeNode Rect", &debugContext.ShowTreeNodeRect);

		return changed;
	}

	bool SceneEditorLayer::drawSceneEntityHierarchyPanelSimple(Scene &scene, DYEditor::Entity *pCurrentSelectedEntity)
	{
		bool changed = false;

		// Draw scene hierarchy context menu.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Create Empty"))
			{
				// Select the newly created entity.
				// For now, the entity is always put at the end of the list.
				*pCurrentSelectedEntity = scene.World.CreateEntity("Entity");
				int const indexInWorldArray = scene.World.GetNumberOfEntities() - 1;
				Undo::RegisterEntityCreation(scene.World, *pCurrentSelectedEntity, indexInWorldArray);
				changed = true;
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

		auto itemSpacing = ImGui::GetStyle().ItemSpacing;
		itemSpacing.y = 6;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing);

		struct HierarchyLevel
		{
			DYE::GUID LevelParentGUID = (DYE::GUID) 0;
			int NumberOfEntitiesLeft = 0;
			bool IsOpen = false;
		};
		std::vector<HierarchyLevel> levelStack;
		// We reserve to make sure the array doesn't re-allocate during foreach.
		levelStack.reserve(scene.World.GetNumberOfEntities());

		// Draw all entities.
		scene.World.ForEachEntityAndIndex
			(
				[&changed, &scene, &pCurrentSelectedEntity, &levelStack]
					(DYEditor::Entity &entity, std::size_t indexInWorld)
				{
					auto tryGetNameResult = entity.TryGetName();
					if (!tryGetNameResult.has_value())
					{
						// No name, skip it.
						return;
					}

					auto tryGetGUIDResult = entity.TryGetGUID();
					if (!tryGetGUIDResult.has_value())
					{
						// No GUID, skip it.
						return;
					}

					auto &name = tryGetNameResult.value();
					auto guid = tryGetGUIDResult.value();
					auto tryGetChildrenComponent = entity.TryGetComponent<ChildrenComponent>();

					auto tryGetParent = entity.TryGetComponent<ParentComponent>();
					auto parentGUID = tryGetParent.has_value()? tryGetParent.value().get().ParentGUID : (DYE::GUID) 0;

					int childrenCount = tryGetChildrenComponent.has_value()? tryGetChildrenComponent.value().get().ChildrenGUIDs.size() : 0;

					HierarchyLevel *pLevel = levelStack.empty()? nullptr : &levelStack.back();
					while
					(
						pLevel != nullptr &&
						pLevel->LevelParentGUID != parentGUID &&
						pLevel->NumberOfEntitiesLeft == 0
					)
					{
						// The top level is not the parent of the current entity
						// && It's also an empty level (all the entities of that level have been iterated)
						// -> We will pop the top level.

						// If the tree node of the level is open,
						// we need to pop it.
						if (pLevel->IsOpen)
						{
							ImGui::TreePop();
						}

						// Pop the level from the stack.
						levelStack.pop_back();

						pLevel = levelStack.empty()? nullptr : &levelStack.back();
					}

					bool const isEntityShown = pLevel == nullptr || pLevel->IsOpen;
					bool const isLeafNode = childrenCount == 0;

					bool const isSelected = entity == *pCurrentSelectedEntity;

					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
					if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
					if (isLeafNode)
					{
						flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
					}

					void const* treeNodeId = (void *) (std::uint64_t) entity.GetInstanceID();
					bool isNodeOpen = false;
					if (isEntityShown)
					{
						isNodeOpen = ImGui::TreeNodeEx(treeNodeId, flags, name.c_str()) && !isLeafNode;
						if (ImGui::BeginPopupContextItem())
						{
							// Draw entity context menu right after TreeNode call.
							if (ImGui::Selectable("Delete"))
							{
								Undo::DeleteEntity(scene.World, entity, indexInWorld);
								changed = true;
							}
							if (ImGui::Selectable("Create Empty"))
							{
								// Select the newly created entity.
								// For now, the entity is always put at the end of the list.
								// TODO: make it a child of the clicked tree node entity.
								*pCurrentSelectedEntity = scene.World.CreateEntity("Entity");
								int const indexInWorldArray = scene.World.GetNumberOfEntities() - 1;
								Undo::RegisterEntityCreation(scene.World, *pCurrentSelectedEntity, indexInWorldArray);
								changed = true;
							}
							ImGui::EndPopup();
						}
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						{
							*pCurrentSelectedEntity = entity;
						}
					}

					if (pLevel != nullptr)
					{
						pLevel->NumberOfEntitiesLeft--;
					}

					if (childrenCount > 0)
					{
						levelStack.push_back
						(
							{
								.LevelParentGUID = guid,
								.NumberOfEntitiesLeft = childrenCount,
								.IsOpen = isNodeOpen
							}
						);
					}
				}
			);

		for (auto level : levelStack)
		{
			// There are some levels left in the stack,
			// we need to make sure to pop the corresponding tree nodes if they are open.
			if (!level.IsOpen)
			{
				continue;
			}

			ImGui::TreePop();
		}

		ImGui::PopStyleVar();


		return changed;
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
						SystemDescriptor descriptor =
						{
							.Name = systemName,
							.Group = NoSystemGroupID,
							.IsEnabled = true,
							.Instance = pSystemInstance
						};
						Undo::AddSystem(scene, descriptor, systemDescriptors.size());
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
			SystemBase* pSystemInstance = systemDescriptor.Instance;
			bool const isRecognizedSystem = pSystemInstance != nullptr;
			char const* headerText = isRecognizedSystem ? systemDescriptor.Name.c_str() : "(Unrecognized System)";

			ImGui::AlignTextToFramePadding();

			bool isHeaderVisible = true;
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.203f, 0.203f, 0.276f, 1.000f));
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
			bool isSystemEnabled = systemDescriptor.IsEnabled;
			ImGui::Checkbox("##IsEnabled", &isSystemEnabled);
			if (isSystemEnabled != systemDescriptor.IsEnabled)
			{
				Undo::SetSystemIsEnabled(scene, systemDescriptor, i, isSystemEnabled);
			}

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
					Undo::ReorderSystem(scene, systemDescriptor, i, otherSystemIndex);
					changed = true;
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
					Undo::ReorderSystem(scene, systemDescriptor, i, otherSystemIndex);
					changed = true;
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
			Undo::RemoveSystem(scene, systemDescriptors[indexToRemove], indexToRemove);
		}

		return changed;
	}

	bool SceneEditorLayer::drawEntityInspector(EntityInspectorContext &context,
											   std::vector<std::pair<std::string, ComponentTypeDescriptor>> componentNamesAndDescriptors)
	{
		Entity &entity = context.Entity;
		InspectorMode &mode = context.Mode;

		if (!entity.IsValid())
		{
			return false;
		}

		bool isEntityChangedThisFrame = false;

		ImVec2 const addComponentButtonSize = ImVec2 {120, 0};
		float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY? ImGui::GetWindowScrollbarRect(ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;

		// Draw entity's NameComponent as a InputField on the top.
		auto& nameComponent = entity.AddOrGetComponent<NameComponent>();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() - scrollBarWidth - addComponentButtonSize.x - ImGui::GetFontSize());
		{
			bool const changedThisFrame = ImGui::InputText("##EntityNameComponent", &nameComponent.Name);
			if (ImGui::IsItemActivated())
			{
				context.IsModifyingEntityProperty = true;
				context.SerializedComponentBeforeModification =
					SerializedObjectFactory::CreateSerializedComponentOfType(entity, NameComponentName, TypeRegistry::GetComponentTypeDescriptor_NameComponent());
			}

			if (ImGui::IsItemDeactivated())
			{
				context.IsModifyingEntityProperty = false;
			}
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				auto serializedNameComponentAfterModification =
					SerializedObjectFactory::CreateSerializedComponentOfType(entity, NameComponentName, TypeRegistry::GetComponentTypeDescriptor_NameComponent());
				Undo::RegisterComponentModification(entity, context.SerializedComponentBeforeModification, serializedNameComponentAfterModification);
			}

			isEntityChangedThisFrame |= changedThisFrame;
		}
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
				for (auto const& [typeName, typeDescriptor] : componentNamesAndDescriptors)
				{
					if (mode == InspectorMode::Normal && !typeDescriptor.ShouldBeIncludedInNormalAddComponentList)
					{
						continue;
					}

					if (typeDescriptor.Has(entity))
					{
						// The entity already has this component, skip it.
						continue;
					}

					if (ImGui::Selectable(typeName.c_str()))
					{
						// Add the component.
						Undo::AddComponent(entity, typeName, typeDescriptor);
						isEntityChangedThisFrame = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndPopup();
		}

		// Draw all components that the entity has.
		for (auto& [typeName, typeDescriptor] : componentNamesAndDescriptors)
		{
			if (mode == InspectorMode::Normal && !typeDescriptor.ShouldDrawInNormalInspector)
			{
				continue;
			}

			DYE_ASSERT_LOG_WARN(typeDescriptor.Has != nullptr, "Missing 'Has' function for component '%s'.", typeName.c_str());
			if (!typeDescriptor.Has(entity))
			{
				continue;
			}

			bool isHeaderVisible = true;
			bool showComponentInspector = true;

			ImGui::PushID(typeName.c_str());
			if (typeDescriptor.DrawHeader == nullptr)
			{
				ImGuiTreeNodeFlags const flags = ImGuiTreeNodeFlags_DefaultOpen;
				showComponentInspector = ImGui::CollapsingHeader("##Header", &isHeaderVisible, flags);

				// Spacing ahead of the component name.
				float const spacing = ImGui::GetFrameHeight();
				ImGui::SameLine(); ImGui::ItemSize(ImVec2(spacing, 0));

				// The name of the component.
				ImGui::SameLine();
				ImGui::TextUnformatted(typeName.c_str());

			}
			else
			{
				// Use custom header drawer if provided.
				DrawComponentHeaderContext drawHeaderContext;
				showComponentInspector = typeDescriptor.DrawHeader(drawHeaderContext, entity, isHeaderVisible, typeName);
				if (drawHeaderContext.IsModificationActivated)
				{
					context.IsModifyingEntityProperty = true;
					context.SerializedComponentBeforeModification =
						SerializedObjectFactory::CreateSerializedComponentOfType(entity, typeName, typeDescriptor);
				}

				if (drawHeaderContext.IsModificationDeactivated)
				{
					context.IsModifyingEntityProperty = false;
				}
				if (drawHeaderContext.IsModificationDeactivatedAfterEdit)
				{
					auto serializedComponentAfterModification = SerializedObjectFactory::CreateSerializedComponentOfType(entity, typeName, typeDescriptor);
					Undo::RegisterComponentModification(entity, context.SerializedComponentBeforeModification, serializedComponentAfterModification);
				}

				isEntityChangedThisFrame |= drawHeaderContext.ComponentChanged;
			}
			ImGui::PopID();

			bool const isRemoved = !isHeaderVisible;
			if (isRemoved)
			{
				// Remove the component
				Undo::RemoveComponent(entity, typeName, typeDescriptor);
				isEntityChangedThisFrame = true;
				continue;
			}

			if (!showComponentInspector)
			{
				continue;
			}

			if (typeDescriptor.DrawInspector == nullptr)
			{
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextWrapped("Missing 'DrawInspector' function for component '%s'. "
									   "It's likely that the DrawInspectorFunction is not assigned when TypeRegistry::registerComponentType is called.", typeName.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}
			else
			{
				ImGui::PushID(typeName.c_str());
				DrawComponentInspectorContext drawComponentInspectorContext;
				isEntityChangedThisFrame |= typeDescriptor.DrawInspector(drawComponentInspectorContext, entity);

				if (drawComponentInspectorContext.IsModificationActivated)
				{
					context.IsModifyingEntityProperty = true;
					context.SerializedComponentBeforeModification =
						SerializedObjectFactory::CreateSerializedComponentOfType(entity, typeName, typeDescriptor);
				}

				if (drawComponentInspectorContext.IsModificationDeactivated)
				{
					context.IsModifyingEntityProperty = false;
				}
				if (drawComponentInspectorContext.IsModificationDeactivatedAfterEdit)
				{
					auto serializedComponentAfterModification = SerializedObjectFactory::CreateSerializedComponentOfType(entity, typeName, typeDescriptor);
					Undo::RegisterComponentModification(entity, context.SerializedComponentBeforeModification, serializedComponentAfterModification);
				}

				ImGui::PopID();
			}

			ImGui::Spacing();
		}

		// Finally, if the entity has EntityDeserializationResult, we want to draw the deserialization report in the inspector.
		// (i.e. Unrecognized components etc)
		if (entity.HasComponent<EntityDeserializationResult>())
		{
			EntityDeserializationResult &deserializationResult = entity.GetComponent<EntityDeserializationResult>();

			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.000f, 0.000f, 0.000f, 0.310f));
			bool const showUnrecognizedSystems = ImGui::CollapsingHeader("Unrecognized Components");
			ImGui::PopStyleColor();
			if (showUnrecognizedSystems)
			{
				if (ImGui::BeginTable("Unrecognized Component Table", 1, ImGuiTableFlags_RowBg))
				{
					int indexToRemove = -1;
					for (int i = 0; i < deserializationResult.UnrecognizedComponentTypeNames.size(); ++i)
					{
						auto const &typeName = deserializationResult.UnrecognizedComponentTypeNames[i];

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::PushID(typeName.c_str());

						char label[128];
						sprintf(label, "Component '%s' is unknown in the TypeRegistry.", typeName.c_str());
						ImGui::Bullet();
						ImGui::Selectable(label, false);

						char const *popupId = "Unknown component popup";
						if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						{
							ImGui::OpenPopup(popupId);
						}

						if (ImGui::BeginPopup(popupId))
						{
							if (ImGui::Selectable("Copy Name"))
							{
								ImGui::SetClipboardText(typeName.c_str());
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
						deserializationResult.UnrecognizedComponentTypeNames.erase(deserializationResult.UnrecognizedComponentTypeNames.begin() + indexToRemove);
						deserializationResult.UnrecognizedSerializedComponents.erase(deserializationResult.UnrecognizedSerializedComponents.begin() + indexToRemove);
					}

					ImGui::EndTable();
				}
			}
		}

		return isEntityChangedThisFrame;
	}
}