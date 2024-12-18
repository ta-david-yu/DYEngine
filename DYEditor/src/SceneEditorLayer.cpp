#include "SceneEditorLayer.h"

#include "SceneRuntimeLayer.h"
#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/RuntimeState.h"
#include "Core/RuntimeSceneManagement.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Configuration/ProjectConfig.h"
#include "Configuration/SubWindowConfiguration.h"
#include "Type/BuiltInTypeRegister.h"
#include "Type/UserTypeRegister.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/WindowManager.h"
#include "Graphics/Framebuffer.h"
#include "Input/InputManager.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"
#include "Util/EntityUtil.h"
#include "ImGui/EditorWindowManager.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/ImGuiUtil_Internal.h"
#include "Undo/Undo.h"
#include "Undo/UndoOperationBase.h"
#include "Math/Math.h"
#include "Audio/AudioManager.h"
#include "SceneViewEntitySelection.h"
#include "Util/StringUtil.h"

#include "Core/Components.h"
#include "Core/Systems.h"

#include <filesystem>
#include <unordered_set>
#include <stack>
#include <iostream>
#include <execution>
#include <algorithm>
#include <regex>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

using namespace DYE::DYEditor;

namespace DYE::DYEditor
{
    constexpr char const *k_DYEditorWindowId = "###DYEditor";
    constexpr char const *k_DYEditorDockSpaceId = "DYEditor DockSpace";
    constexpr char const *k_SceneHierarchyWindowId = "Scene Hierarchy";
    constexpr char const *k_SceneSystemWindowId = "Scene System";
    constexpr char const *k_EntityInspectorWindowId = "###EntityInspector";
    constexpr char const *k_SceneViewWindowId = "Scene View";

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
                    .Name = "Audio Manager",
                    .isConfigOpenByDefault = false
                },
            [](char const *name, bool *pIsOpen, ImGuiViewport const *pMainViewportHint)
            {
                AudioManager::DrawAudioManagerImGui(pIsOpen);
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
                DrawEditorConfigurationWindow(pIsOpen);
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
        std::string const &defaultScenePath = GetEditorConfig().GetOrDefault<std::string>(EditorConfigKeys::DefaultScene, "");
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
            initializeNewSceneWithDefaultEntityAndSystems(RuntimeSceneManagement::GetActiveMainScene());
        }

        // Register runtime state events.
        RuntimeState::RegisterListener(this);

        // Initialize SceneViewEntitySelection shader.
        SceneViewEntitySelection::InitializeEntityIDShader();

        // Initialize SceneView frameBuffers & camera.
        FramebufferProperties targetFramebufferProperties {.Width = 1600, .Height = 900};
        targetFramebufferProperties.Attachments =
            {
                FramebufferTextureFormat::RGBA8,
                FramebufferTextureFormat::Depth
            };
        m_SceneViewCameraTargetFramebuffer = Framebuffer::Create(targetFramebufferProperties);
        m_SceneViewCameraTargetFramebuffer->SetDebugLabel("Scene View Framebuffer");

        FramebufferProperties entityIDFramebufferProperties {.Width = 1600, .Height = 900};
        entityIDFramebufferProperties.Attachments =
            {
                FramebufferTextureFormat::RedInteger,
                FramebufferTextureFormat::Depth
            };
        m_SceneViewEntityIDFramebuffer = Framebuffer::Create(entityIDFramebufferProperties);
        m_SceneViewEntityIDFramebuffer->SetDebugLabel("Scene View EntityID Framebuffer");

        // Do some more editor setup based on EditorConfig settings.
        ProjectConfig &editorConfig = GetEditorConfig();
        m_InspectorContext.Mode = editorConfig.GetOrDefault(EditorConfigKeys::DebugInspector, false) ? InspectorMode::Debug : InspectorMode::Normal;
        m_SceneViewCamera.Properties.TargetType = RenderTargetType::RenderTexture;
        m_SceneViewCamera.Properties.pTargetRenderTexture = m_SceneViewCameraTargetFramebuffer.get();
        m_SceneViewCamera.SetPosition(editorConfig.GetOrDefault(EditorConfigKeys::SceneViewCameraPosition, glm::vec3 {0, 0, 10.0f}));
        m_SceneViewCamera.Properties.ClearColor = editorConfig.GetOrDefault(EditorConfigKeys::SceneViewCameraClearColor, glm::vec4 {0, 0, 0, 1});

        bool const shouldSetupSubWindowsBasedOnRuntimeConfig = editorConfig.GetOrDefault(EditorConfigKeys::ShowSubWindowsInEditMode, false);
        if (shouldSetupSubWindowsBasedOnRuntimeConfig)
        {
            SetupSubWindowsBasedOnRuntimeConfig();
        }
    }

    void SceneEditorLayer::OnDetach()
    {
        // Unregister events.
        RuntimeState::UnregisterListener(this);

        // Save current active scene as default scene for the next launch.
        ProjectConfig &editorConfig = GetEditorConfig();
        editorConfig.Set<std::string>(EditorConfigKeys::DefaultScene, m_CurrentSceneFilePath.string());
        editorConfig.Set<glm::vec3>(EditorConfigKeys::SceneViewCameraPosition, m_SceneViewCamera.GetPosition());
        editorConfig.Set<glm::vec4>(EditorConfigKeys::SceneViewCameraClearColor, m_SceneViewCamera.Properties.ClearColor);
        editorConfig.Save();

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

        auto sceneViewCameraPosition = m_SceneViewCamera.GetPosition();
        // Scene View Camera Input.
        if (INPUT.GetKey(KeyCode::Up))
        {
            sceneViewCameraPosition.y += m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
        }
        if (INPUT.GetKey(KeyCode::Down))
        {
            sceneViewCameraPosition.y -= m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
        }
        if (INPUT.GetKey(KeyCode::Right))
        {
            sceneViewCameraPosition.x += m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
        }
        if (INPUT.GetKey(KeyCode::Left))
        {
            sceneViewCameraPosition.x -= m_CameraKeyboardMoveUnitPerSecond * TIME.DeltaTime();
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
            sceneViewCameraPosition += glm::vec3 {-panMove.x, panMove.y, 0};
        }
        m_SceneViewCamera.SetPosition(sceneViewCameraPosition);

        SceneViewEntitySelection::ReceiveEntityGeometrySubmission(m_IsSceneViewDrawn);
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
            auto mouseScrolledEvent = (MouseScrolledEvent &) event;
            m_SceneViewCamera.Properties.OrthographicSize -= TIME.DeltaTime() * m_CameraOrthographicSizeZoomSpeedMultiplier * mouseScrolledEvent.GetY();
            if (m_SceneViewCamera.Properties.OrthographicSize < 0.1f)
            {
                m_SceneViewCamera.Properties.OrthographicSize = 0.1f;
            }

            // Use the event up.
            mouseScrolledEvent.IsUsed = true;
        }

        if (event.GetEventType() == EventType::KeyDown)
        {
            auto keyDownEvent = (KeyDownEvent &) event;
            bool const control = INPUT.GetKey(KeyCode::LeftControl) || INPUT.GetKey(KeyCode::RightControl);
            bool const shift = INPUT.GetKey(KeyCode::LeftShift) || INPUT.GetKey(KeyCode::RightShift);
            bool const isUsingGizmo = ImGuizmo::IsUsing();

            Scene &activeScene = RuntimeSceneManagement::GetActiveMainScene();

            // Editor Shortcuts.
            switch (keyDownEvent.GetKeyCode())
            {
                case KeyCode::D:
                {
                    if (control)
                    {
                        // Ctrl + D: Duplicate Entity
                        if (!m_IsSceneHierarchyWindowFocused && !m_IsSceneViewWindowFocused)
                        {
                            break;
                        }

                        auto tryGetSelectedEntity = activeScene.World.TryGetEntityWithGUID(m_CurrentlySelectedEntityGUID);
                        if (tryGetSelectedEntity.has_value())
                        {
                            Entity selectedEntity = tryGetSelectedEntity.value();

                            std::string name = selectedEntity.TryGetName().value();
                            name = StringUtil::AddOrIncrementNumberSuffix(name);

                            auto newEntity = Undo::DuplicateEntityRecursively(activeScene.World, selectedEntity, name);
                            m_CurrentlySelectedEntityGUID = newEntity.TryGetGUID().value();

                            m_IsActiveSceneDirty = true;
                        }
                    }
                    break;
                }

                case KeyCode::Q:
                {
                    if (isUsingGizmo)
                    {
                        break;
                    }
                    m_SceneViewContext.GizmoType = -1;
                    break;
                }
                case KeyCode::W:
                {
                    if (isUsingGizmo)
                    {
                        break;
                    }
                    m_SceneViewContext.GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                    break;
                }
                case KeyCode::E:
                {
                    if (isUsingGizmo)
                    {
                        break;
                    }
                    m_SceneViewContext.GizmoType = ImGuizmo::OPERATION::ROTATE;
                    break;
                }
                case KeyCode::R:
                {
                    if (isUsingGizmo)
                    {
                        break;
                    }
                    m_SceneViewContext.GizmoType = ImGuizmo::OPERATION::SCALE;
                    break;
                }

                case KeyCode::F:
                {
                    bool const isRelatedWindowFocused = m_IsSceneViewWindowFocused || m_IsSceneHierarchyWindowFocused;
                    if (!isRelatedWindowFocused)
                    {
                        break;
                    }

                    auto tryGetSelectedEntity = activeScene.World.TryGetEntityWithGUID(m_CurrentlySelectedEntityGUID);
                    if (!tryGetSelectedEntity.has_value())
                    {
                        break;
                    }

                    FocusSceneViewCameraToEntity(tryGetSelectedEntity.value());

                    break;
                }

                case KeyCode::N:
                {
                    if (shift && control)
                    {
                        // Shift + Ctrl + N: 'New Entity'
                        auto newEntity = activeScene.World.CreateEntity("Entity");
                        m_CurrentlySelectedEntityGUID = newEntity.TryGetGUID().value();
                        int const newEntityIndexInWorld = activeScene.World.GetNumberOfEntities() - 1;
                        Undo::RegisterEntityCreation(activeScene.World, newEntity, newEntityIndexInWorld);

                        m_IsActiveSceneDirty = true;
                    }
                    else if (control)
                    {
                        // Ctrl + N: 'New Scene'
                        m_CurrentSceneFilePath.clear();
                        activeScene.Clear();

                        initializeNewSceneWithDefaultEntityAndSystems(activeScene);

                        Undo::ClearAll();
                        m_IsActiveSceneDirty = false;
                    }
                    break;
                }

                case KeyCode::O:
                {
                    if (control)
                    {
                        // Ctrl + O: 'Load Scene'
                        m_OpenLoadSceneFilePopup = true;
                    }
                    break;
                }

                case KeyCode::P:
                {
                    if (control)
                    {
                        RuntimeState::SetIsPlayingAtTheEndOfFrame(!RuntimeState::IsPlaying());
                    }
                    break;
                }

                case KeyCode::S:
                {
                    if (shift && control)
                    {
                        // Shift + Ctrl + S: 'Save Scene as...'
                        m_OpenSaveSceneFilePopup = true;
                    }
                    else if (control)
                    {
                        // Ctrl + S
                        if (m_CurrentSceneFilePath.empty())
                        {
                            // If the context filepath is empty, it's the same as 'Save Scene as...'.
                            m_OpenSaveSceneFilePopup = true;
                        }
                        else
                        {
                            auto serializedScene = SerializedObjectFactory::CreateSerializedScene(activeScene);
                            SerializedObjectFactory::SaveSerializedSceneToFile(serializedScene, m_CurrentSceneFilePath);
                            m_IsActiveSceneDirty = false;
                        }
                    }
                    break;
                }

                case KeyCode::Y:
                {
                    if (isUsingGizmo)
                    {
                        break;
                    }

                    if (control)
                    {
                        // Ctrl + Y
                        Undo::PerformRedo();
                    }
                }
                case KeyCode::Z:
                {
                    if (isUsingGizmo)
                    {
                        break;
                    }

                    if (control && shift)
                    {
                        // Ctrl + Shift + Z
                        Undo::PerformRedo();
                    }
                    else if (control)
                    {
                        // Ctrl + Z
                        Undo::PerformUndo();
                    }
                    else if (!control && !shift)
                    {
                        // Z
                        m_SceneViewContext.IsGizmoLocalSpace = !m_SceneViewContext.IsGizmoLocalSpace;
                    }

                    break;
                }

                case KeyCode::Delete:
                {
                    // Delete: Delete Entity
                    if (!m_IsSceneHierarchyWindowFocused && !m_IsSceneViewWindowFocused)
                    {
                        break;
                    }

                    auto tryGetSelectedEntity = activeScene.World.TryGetEntityWithGUID(m_CurrentlySelectedEntityGUID);
                    if (tryGetSelectedEntity.has_value())
                    {
                        Entity selectedEntity = tryGetSelectedEntity.value();
                        Undo::DeleteEntityRecursively(selectedEntity);
                        m_IsActiveSceneDirty = true;
                    }
                    break;
                }
            }
            // Use the event up.
            keyDownEvent.IsUsed = true;
        }
    }

    void SceneEditorLayer::OnRender()
    {
        if (m_IsSceneViewDrawn)
        {
            // We only want to render the scene view camera if the scene view window is drawn.

            // Register scene view camera to render what the user sees in scene view.
            DYE::RenderPipelineManager::RegisterCameraForNextRender(m_SceneViewCamera);

            // Directly render entity id framebuffer for entity mouse selection.
            SceneViewEntitySelection::RenderEntityIDFramebufferWithCamera(*m_SceneViewEntityIDFramebuffer, m_SceneViewCamera);
        }

        // Clear entity geometries.
        SceneViewEntitySelection::ClearEntityGeometries();
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

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float const editorLayerWindowPadding = 10;
        ImVec2 const editorLayerWindowPos = {viewport->WorkPos.x + editorLayerWindowPadding, viewport->WorkPos.y};
        ImVec2 const editorLayerWindowSize = {viewport->WorkSize.x - editorLayerWindowPadding * 2, viewport->WorkSize.y - editorLayerWindowPadding};
        ImGui::SetNextWindowPos(editorLayerWindowPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(editorLayerWindowSize, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        char mainEditorName[32];
        sprintf(mainEditorName, "%s%s", RuntimeState::IsPlaying() ? "DYEditor (Play Mode)" : "DYEditor (Edit Mode)", k_DYEditorWindowId);
        ImGui::Begin(mainEditorName, nullptr, mainEditorWindowFlags);
        ImGui::PopStyleVar();

        drawEditorWindowMenuBar(activeScene, m_CurrentSceneFilePath, &m_IsActiveSceneDirty, m_OpenLoadSceneFilePopup, m_OpenSaveSceneFilePopup);

        ImGuiIO &io = ImGui::GetIO();
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

        // Scene View
        ImGuiWindowFlags const sceneViewWindowFlags = ImGuiWindowFlags_MenuBar;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0, 0});
        ImGui::SetNextWindowBgAlpha(0.35f);
        m_IsSceneViewDrawn = ImGui::Begin(k_SceneViewWindowId, nullptr, sceneViewWindowFlags);
        m_IsSceneViewWindowFocused = ImGui::IsWindowFocused();
        m_IsSceneViewWindowHovered = ImGui::IsWindowHovered();
        if (m_IsSceneViewDrawn)
        {
            auto tryGetSelectedEntity = activeScene.World.TryGetEntityWithGUID(m_CurrentlySelectedEntityGUID);
            auto selectedEntity = tryGetSelectedEntity.has_value() ? tryGetSelectedEntity.value() : Entity::Null();
            bool const sceneViewChanged = drawSceneView(m_SceneViewCamera, *m_SceneViewEntityIDFramebuffer, selectedEntity, m_SceneViewContext);
            m_IsActiveSceneDirty |= sceneViewChanged;
        }

        // ImGuiLayer shouldn't block events when SceneView window is focused OR hovered.
        bool const editorShouldReceiveCameraInputEvent = m_IsSceneViewDrawn && (m_IsSceneViewWindowFocused || m_IsSceneViewWindowHovered);
        m_pApplication->GetImGuiLayer().SetBlockEvents(!editorShouldReceiveCameraInputEvent);

        ImGui::End();
        ImGui::PopStyleVar();

        // System Panel
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin(k_SceneSystemWindowId))
        {
            bool const isSystemListModified = drawSceneSystemPanel(activeScene);
            m_IsActiveSceneDirty |= isSystemListModified;
        }
        ImGui::End();

        // Scene Hierarchy
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGuiWindowFlags const hierarchyWindowFlags = m_IsActiveSceneDirty ? ImGuiWindowFlags_UnsavedDocument : ImGuiWindowFlags_None;
        bool const isSceneHierarchyDrawn = ImGui::Begin(k_SceneHierarchyWindowId, nullptr, hierarchyWindowFlags);
        m_IsSceneHierarchyWindowFocused = ImGui::IsWindowFocused();
        if (isSceneHierarchyDrawn)
        {
            bool const isHierarchyChanged = drawSceneEntityHierarchyPanel(activeScene, &m_CurrentlySelectedEntityGUID, m_EntityNodesToBeOpenInHierarchy);
            m_IsActiveSceneDirty |= isHierarchyChanged;
        }
        ImGui::End();

        // Entity Inspector
        ImGui::SetNextWindowBgAlpha(0.35f);
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
                    GetEditorConfig().SetAndSave(EditorConfigKeys::DebugInspector, false);
                }

                if (ImGui::MenuItem("Debug", nullptr, debugMode))
                {
                    m_InspectorContext.Mode = InspectorMode::Debug;
                    GetEditorConfig().SetAndSave(EditorConfigKeys::DebugInspector, true);
                }

                ImGui::EndPopup();
            }

            auto tryGetSelectedEntity = activeScene.World.TryGetEntityWithGUID(m_CurrentlySelectedEntityGUID);
            m_InspectorContext.ShouldEarlyOutIfInIteratorLoop = false; // We always want to reset this value from the last frame.
            m_InspectorContext.Entity = tryGetSelectedEntity.has_value() ? tryGetSelectedEntity.value() : Entity::Null();
            bool const isEntityChanged = drawEntityInspector(m_InspectorContext, TypeRegistry::GetComponentTypesNamesAndDescriptors());
            m_IsActiveSceneDirty |= isEntityChanged;
        }
        ImGui::End();

        // Scene Load/Save Dialog.
        drawOpenSceneDialogWindow(activeScene, m_CurrentSceneFilePath, &m_IsActiveSceneDirty, m_OpenLoadSceneFilePopup, m_OpenSaveSceneFilePopup);
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

    void SceneEditorLayer::drawEditorWindowMenuBar(Scene &currentScene,
                                                   std::filesystem::path &currentScenePathContext,
                                                   bool *pIsSceneDirty,
                                                   bool &openLoadSceneFilePathPopup,
                                                   bool &openSaveSceneFilePathPopup)
    {
        char const *loadScenePopupId = "Select a scene file (*.tscene)";
        char const *saveScenePopupId = "Save scene file as... ";

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N", false, !RuntimeState::IsPlaying()))
                {
                    currentScenePathContext.clear();
                    currentScene.Clear();

                    initializeNewSceneWithDefaultEntityAndSystems(currentScene);

                    Undo::ClearAll();
                    *pIsSceneDirty = false;
                }

                if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, !RuntimeState::IsPlaying()))
                {
                    // We store a flag here and delay opening the popup
                    // because MenuItem is Selectable and Selectable by default calls CloseCurrentPopup().
                    openLoadSceneFilePathPopup = true;
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
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

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
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
                    [](EditorWindow &editorWindow)
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
    }

    bool SceneEditorLayer::drawSceneView(Camera &sceneViewCamera, Framebuffer &entityIDFramebuffer, Entity selectedEntity, SceneViewContext &context)
    {
        bool changed = false;

        // Update scene viewport bounds.
        {
            ImVec2 viewportOffset = ImGui::GetCursorPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 viewportBoundsMin = ImGui::GetWindowPos();
            // We need to offset the values by the cursor position because
            // scene view window could also draw tab/menu bar on the top,
            // and we don't want to take those widgets into account.
            windowSize.x -= viewportOffset.x;
            windowSize.y -= viewportOffset.y;
            viewportBoundsMin.x += viewportOffset.x;
            viewportBoundsMin.y += viewportOffset.y;
            context.ViewportBounds = Math::Rect(viewportBoundsMin.x, viewportBoundsMin.y, windowSize.x, windowSize.y);
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {8, 8});
        if (ImGui::BeginMenuBar())
        {
            ImGui::SetNextWindowSize({400, 0});
            if (ImGui::BeginMenu("Camera"))
            {
                // Scene View Camera Settings
                ImGui::PushID("Scene View Camera");

                auto position = sceneViewCamera.GetPosition();
                if (ImGuiUtil::DrawVector3Control("Position", position))
                {
                    sceneViewCamera.SetPosition(position);
                }
                ImGuiUtil::DrawCameraPropertiesControl("Properties", sceneViewCamera.Properties);

                ImGui::PopID();
                ImGui::EndMenu();
            }

            ImGui::PushID("Gizmo");
            if (ImGui::BeginMenu("Gizmo"))
            {
                if (ImGui::MenuItem("None", "Q", context.GizmoType == -1))
                {
                    context.GizmoType = -1;
                }
                if (ImGui::MenuItem("Translate", "W", context.GizmoType == ImGuizmo::TRANSLATE))
                {
                    context.GizmoType = ImGuizmo::TRANSLATE;
                }
                if (ImGui::MenuItem("Rotate", "E", context.GizmoType == ImGuizmo::ROTATE))
                {
                    context.GizmoType = ImGuizmo::ROTATE;
                }
                if (ImGui::MenuItem("Scale", "R", context.GizmoType == ImGuizmo::SCALE))
                {
                    context.GizmoType = ImGuizmo::SCALE;
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Local", "Z", context.IsGizmoLocalSpace))
                {
                    context.IsGizmoLocalSpace = !context.IsGizmoLocalSpace;
                }

                ImGui::EndMenu();
            }
            ImGui::PopID();

            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();

        // Render SceneView as a texture image.
        ImVec2 const sceneViewWindowSize = ImGui::GetContentRegionAvail();

        if (sceneViewWindowSize.x <= 0 || sceneViewWindowSize.y <= 0)
        {
            // If either width or height is 0, we don't need to draw the texture OR resize the framebuffer.
            // Because the scene view window is very likely folded.
            return changed;
        }

        auto const &renderTextureProperties = sceneViewCamera.Properties.pTargetRenderTexture->GetProperties();
        if (renderTextureProperties.Width != sceneViewWindowSize.x || renderTextureProperties.Height != sceneViewWindowSize.y)
        {
            // Resize the target framebuffer (render texture) if the window avail region size is different with the framebuffer size.
            sceneViewCamera.Properties.pTargetRenderTexture->Resize(sceneViewWindowSize.x, sceneViewWindowSize.y);
            entityIDFramebuffer.Resize(sceneViewWindowSize.x, sceneViewWindowSize.y);
        }

        auto sceneViewRenderTextureID = sceneViewCamera.Properties.pTargetRenderTexture->GetColorAttachmentID(0);
        auto imTexID = (void *) (intptr_t) (sceneViewRenderTextureID);
        ImVec2 const uv0 = ImVec2(0, 1);
        ImVec2 const uv1 = ImVec2(1, 0);
        ImGui::Image(imTexID, sceneViewWindowSize, uv0, uv1);

        // Draw selected entity's transformation gizmos.

        if (!selectedEntity.IsValid())
        {
            return changed;
        }

        if (context.GizmoType == -1)
        {
            // No gizmo type is now being used, skip it.
            return changed;
        }

        auto tryGetEntityTransform = selectedEntity.TryGetComponent<LocalTransformComponent>();
        if (!tryGetEntityTransform.has_value())
        {
            // If the entity doesn't have a transform, we don't need to draw the gizmo.
            return changed;
        }

        LocalTransformComponent &transform = tryGetEntityTransform.value().get();

        auto tryGetEntityLocalToWorld = selectedEntity.TryGetComponent<LocalToWorldComponent>();

        glm::mat4 localToWorld = !tryGetEntityLocalToWorld.has_value() ? transform.GetTransformMatrix() : tryGetEntityLocalToWorld.value().get().Matrix;
        glm::mat4 worldToLocal = glm::inverse(localToWorld);
        glm::mat4 localToParent = transform.GetTransformMatrix();
        glm::mat4 parentToWorld = !tryGetEntityLocalToWorld.has_value() ? glm::mat4 {1.0f} : tryGetEntityLocalToWorld.value().get().Matrix * glm::inverse(localToParent);
        glm::mat4 worldToParent = glm::inverse(parentToWorld);

        // End manipulating gizmo if the gizmo was not being used anymore.
        // Make an undo operation!
        bool const hasDeactivatedUseOfGizmoAfterEdit = context.IsTransformManipulatedByGizmo && !ImGuizmo::IsUsing();
        if (hasDeactivatedUseOfGizmoAfterEdit)
        {
            auto serializedModifiedTransform = SerializedObjectFactory::CreateSerializedComponentOfType
                (
                    selectedEntity,
                    LocalTransformComponentTypeName,
                    TypeRegistry::GetComponentTypeDescriptor_LocalTransformComponent()
                );

            Undo::RegisterComponentModification(selectedEntity, context.SerializedTransform, serializedModifiedTransform);
            if (context.GizmoType == ImGuizmo::OPERATION::TRANSLATE)
            {
                Undo::SetLatestOperationDescription(
                    "Move Entity '%s' to (%f, %f, %f)",
                    selectedEntity.TryGetName().value().c_str(),
                    transform.Position.x,
                    transform.Position.y,
                    transform.Position.z);
            }
            else if (context.GizmoType == ImGuizmo::OPERATION::ROTATE)
            {
                Undo::SetLatestOperationDescription("Rotate Entity '%s'", selectedEntity.TryGetName().value().c_str());
            }
            else if (context.GizmoType == ImGuizmo::OPERATION::SCALE)
            {
                Undo::SetLatestOperationDescription(
                    "Scale Entity '%s' to (%f, %f, %f)",
                    selectedEntity.TryGetName().value().c_str(),
                    transform.Scale.x,
                    transform.Scale.y,
                    transform.Scale.z);
            }
            context.IsTransformManipulatedByGizmo = false;
        }

        ImGuizmo::SetOrthographic(sceneViewCamera.Properties.IsOrthographic);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(context.ViewportBounds.X, context.ViewportBounds.Y, context.ViewportBounds.Width, context.ViewportBounds.Height);

        glm::mat4 viewMatrix = sceneViewCamera.ViewMatrix;
        glm::mat4 projectionMatrix = sceneViewCamera.Properties.GetProjectionMatrix(context.ViewportBounds.Width / context.ViewportBounds.Height);

        glm::mat4 newLocalToWorld = localToWorld;
        bool const manipulated = ImGuizmo::Manipulate
            (
                glm::value_ptr(viewMatrix),
                glm::value_ptr(projectionMatrix),
                (ImGuizmo::OPERATION) context.GizmoType,
                context.IsGizmoLocalSpace ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
                glm::value_ptr(newLocalToWorld)
            );

        if (!manipulated)
        {
            return changed;
        }

        if (!context.IsTransformManipulatedByGizmo)
        {
            // Start manipulating gizmo,
            // we create a serialized version of transform for undo/redo later.
            context.SerializedTransform = SerializedObjectFactory::CreateSerializedComponentOfType
                (
                    selectedEntity,
                    LocalTransformComponentTypeName,
                    TypeRegistry::GetComponentTypeDescriptor_LocalTransformComponent()
                );

            context.IsTransformManipulatedByGizmo = true;
        }

        // Apply the transform changes back to the select entity transform.
        // Since the gizmo is manipulating the matrix in world space (i.e., local to world matrix),
        // we need to transform the matrix back to parent local space (i.e., local to parent matrix) first
        // before applying the matrix back to local transform component.
        glm::mat4 newLocalToParent = worldToParent * newLocalToWorld;
        glm::vec3 newLocalPosition;
        glm::vec3 newLocalRotationInEulerAngles;
        glm::vec3 newLocalScale;
        if (Math::DecomposeTransform(newLocalToParent, newLocalPosition, newLocalRotationInEulerAngles, newLocalScale))
        {
            transform.Position = newLocalPosition;
            transform.Rotation = glm::quat(newLocalRotationInEulerAngles);
            transform.Scale = newLocalScale;
        }

        if (tryGetEntityLocalToWorld.has_value())
        {
            tryGetEntityLocalToWorld.value().get().Matrix = newLocalToWorld;
        }

        changed = true;

        return changed;
    }

    bool SceneEditorLayer::drawSceneEntityHierarchyPanel(Scene &scene, DYE::GUID *pCurrentSelectedEntityGUID, std::unordered_set<DYE::GUID> &entityNodesToBeOpenInHierarchy)
    {
        bool changed = false;

        if (ImGui::IsWindowHovered())
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                *pCurrentSelectedEntityGUID = (DYE::GUID) 0;
            }
        }

        // Draw scene hierarchy context menu.
        if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::Selectable("Create Empty"))
            {
                // Select the newly created entity.
                // For now, the entity is always put at the end of the list.
                auto newEntity = scene.World.CreateEntity("Entity");
                *pCurrentSelectedEntityGUID = newEntity.TryGetGUID().value();
                int const newEntityIndexInWorld = scene.World.GetNumberOfEntities() - 1;
                Undo::RegisterEntityCreation(scene.World, newEntity, newEntityIndexInWorld);
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

        struct SetParentAction
        {
            bool HasOperation = false;
            std::size_t SrcIndex = 0;
            std::size_t DstParentIndex = 0;
            std::size_t DstFlatIndexRelativeToParent = 0;
        };
        SetParentAction setParent;

        struct ReorderAtTopHierarchyAction
        {
            bool HasOperation = false;
            std::size_t SrcIndex = 0;
            std::size_t DstIndex = 0;
        };
        ReorderAtTopHierarchyAction reorderAtTop;

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        itemSpacing.y = 6;
        auto framePadding = ImGui::GetStyle().FramePadding;
        framePadding.x = 3;
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
                    &pCurrentSelectedEntityGUID,
                    &levelStack,
                    &setParent,
                    &reorderAtTop,
                    &entityNodesToBeOpenInHierarchy]
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
                    auto parentGUID = tryGetParent.has_value() ? tryGetParent.value().get().GetParentGUID() : (DYE::GUID) 0;

                    std::size_t childrenCount = tryGetChildrenComponent.has_value() ? tryGetChildrenComponent.value().get().GetChildrenCount() : 0;

                    HierarchyLevel *pLevel = levelStack.empty() ? nullptr : &levelStack.back();
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

                        pLevel = levelStack.empty() ? nullptr : &levelStack.back();
                    }

                    bool const isEntityShown = pLevel == nullptr || pLevel->IsOpen;
                    bool const isLeafNode = childrenCount == 0;

                    bool const isSelected = guid == *pCurrentSelectedEntityGUID;

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
                        ImGuiID const nodeId = ImGui::GetCurrentWindow()->GetID(guid.ToString().c_str());

                        if (entityNodesToBeOpenInHierarchy.contains(guid))
                        {
                            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                            entityNodesToBeOpenInHierarchy.erase(guid);
                        }

                        isNodeOpen = ImGui::TreeNodeEx(guid.ToString().c_str(), flags, name.c_str()) && !isLeafNode;
                        if (ImGui::BeginPopupContextItem())
                        {
                            // Draw entity context menu right after TreeNode call.
                            if (ImGui::Selectable("Delete"))
                            {
                                Undo::DeleteEntityRecursively(entity, indexInWorld);
                                changed = true;
                            }
                            if (ImGui::Selectable("Create Empty"))
                            {
                                // Select the newly created entity.
                                // For now, the entity is always put at the end of the list.
                                auto newEntity = scene.World.CreateEntity("Entity");
                                *pCurrentSelectedEntityGUID = newEntity.TryGetGUID().value();
                                int const newEntityIndexInWorld = scene.World.GetNumberOfEntities() - 1;

                                Undo::StartGroupOperation("Create Empty Entity (Temporary)");
                                Undo::RegisterEntityCreation(scene.World, newEntity, newEntityIndexInWorld);
                                Undo::SetEntityParent(newEntity, scene.World.GetNumberOfEntities() - 1, entity, indexInWorld);
                                Undo::EndGroupOperation();
                                Undo::SetLatestOperationDescription("Create Empty Entity Under Entity '%s'", name.c_str());

                                // Insert all parents GUID into the to-be-open set,
                                // so their nodes get expanded in the hierarchy window in the next frame.
                                auto tryGetNewEntityParent = newEntity.TryGetComponent<ParentComponent>();
                                while (tryGetNewEntityParent.has_value())
                                {
                                    DYE::GUID newEntityParentGUID = tryGetNewEntityParent.value().get().GetParentGUID();
                                    entityNodesToBeOpenInHierarchy.insert(newEntityParentGUID);

                                    newEntity = tryGetNewEntityParent.value().get().GetParent(scene.World);
                                    tryGetNewEntityParent = newEntity.TryGetComponent<ParentComponent>();
                                }

                                changed = true;
                            }
                            if (ImGui::Selectable("Duplicate"))
                            {
                                std::string newName = StringUtil::AddOrIncrementNumberSuffix(name);
                                auto newEntity = Undo::DuplicateEntityRecursively(scene.World, entity, newName);
                                *pCurrentSelectedEntityGUID = newEntity.TryGetGUID().value();

                                // Insert all parents GUID into the to-be-open set,
                                // so their nodes get expanded in the hierarchy window in the next frame.
                                auto tryGetNewEntityParent = newEntity.TryGetComponent<ParentComponent>();
                                while (tryGetNewEntityParent.has_value())
                                {
                                    DYE::GUID newEntityParentGUID = tryGetNewEntityParent.value().get().GetParentGUID();
                                    entityNodesToBeOpenInHierarchy.insert(newEntityParentGUID);

                                    newEntity = tryGetNewEntityParent.value().get().GetParent(scene.World);
                                    tryGetNewEntityParent = newEntity.TryGetComponent<ParentComponent>();
                                }
                            }
                            ImGui::EndPopup();
                        }
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                        {
                            *pCurrentSelectedEntityGUID = guid;
                        }

                        ImVec2 const entityTreeNodeSize = ImGui::GetItemRectSize();
                        float const entityWidgetCenterY = entityTreeNodeScreenPos.y + entityTreeNodeSize.y * 0.5f;

                        // Make the entity tree node a drag source.
                        ImGuiDragDropFlags const dragHandleFlags = ImGuiDragDropFlags_None;
                        if (ImGui::BeginDragDropSource(dragHandleFlags))
                        {
                            ImGui::SetDragDropPayload("EntityIndex", &indexInWorld, sizeof(std::size_t));

                            // Preview the entity & its children in the drag tooltip.
                            EntityUtil::ForEntityAndEachChildPreorderWithDepth
                                (
                                    entity,
                                    [](Entity e, int depth)
                                    {
                                        for (int i = 0; i < depth; i++)
                                        {
                                            ImGui::Indent();
                                        }

                                        ImGui::TextUnformatted(e.TryGetName().value().c_str());

                                        for (int i = 0; i < depth; i++)
                                        {
                                            ImGui::Unindent();
                                        }
                                    }
                                );

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
                            float const upperHandleHeight = spacingBetweenTreeNode * 0.5f;
                            float const lowerHandleHeight = spacingBetweenTreeNode * 0.5f;
                            float const middleHandleHeight = entityWidgetSize.y - upperHandleHeight - lowerHandleHeight;

                            ImGui::SetCursorScreenPos(entityWidgetScreenPos);
                            ImGuiUtil::Internal::InteractableItem("EntityDropHandle_Upper",
                                                                  ImVec2(entityWidgetSize.x, upperHandleHeight));
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
                                        bool const hasParent = parentGUID != (DYE::GUID) 0;
                                        auto tryGetParentEntity = entity.GetWorld().TryGetEntityWithGUID(parentGUID);
                                        if (hasParent && tryGetParentEntity.has_value())
                                        {
                                            int parentIndex = entity.GetWorld().TryGetEntityIndex(tryGetParentEntity.value()).value();
                                            setParent.HasOperation = true;
                                            setParent.SrcIndex = payloadIndex;
                                            setParent.DstParentIndex = parentIndex;
                                            setParent.DstFlatIndexRelativeToParent = indexInWorld - parentIndex;
                                        }
                                        else
                                        {
                                            reorderAtTop.HasOperation = true;
                                            reorderAtTop.SrcIndex = payloadIndex;
                                            reorderAtTop.DstIndex = indexInWorld;
                                        }
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }

                            ImGui::SetCursorScreenPos(
                                ImVec2(entityWidgetScreenPos.x, entityWidgetScreenPos.y + upperHandleHeight));
                            ImGuiUtil::Internal::InteractableItem("EntityDropHandle_Middle",
                                                                  ImVec2(entityWidgetSize.x, middleHandleHeight));
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
                                        setParent.HasOperation = true;
                                        setParent.SrcIndex = payloadIndex;
                                        setParent.DstParentIndex = indexInWorld;
                                        setParent.DstFlatIndexRelativeToParent = -1;

                                        // Open/expand the dropped entity (new parent) tree node.
                                        ImGuiID newParentTreeNodeId = ImGui::GetCurrentWindow()->GetID(guid.ToString().c_str());
                                        ImGui::TreeNodeSetOpen(newParentTreeNodeId, true);
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }

                            ImGui::SetCursorScreenPos(
                                ImVec2(entityWidgetScreenPos.x, entityWidgetScreenPos.y + upperHandleHeight + middleHandleHeight));
                            ImGuiUtil::Internal::InteractableItem("EntityDropHandle_Lower",
                                                                  ImVec2(entityWidgetSize.x, lowerHandleHeight));
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
                                        // There could be two behaviours for the lower handle,
                                        //  1. When the node is open, the lower handle will be used to insert the dragged entity as a child of the dropped handle.
                                        //  2. When the node is closed, it will be used to insert below the dropped entity instead of re-parenting.
                                        // Therefore, we need to draw the preview line differently to distinguish the two behaviours.
                                        float const indentSpacing = ImGui::GetStyle().IndentSpacing;
                                        float const previewLineStartX = isNodeOpen ? entityWidgetScreenPos.x + indentSpacing : entityWidgetScreenPos.x;
                                        float const previewLineWidth = isNodeOpen ? entityWidgetSize.x - indentSpacing : entityWidgetSize.x;

                                        ImVec2 const previewLineBegin = ImVec2(previewLineStartX, entityWidgetScreenPos.y + entityWidgetSize.y);
                                        ImVec2 const previewLineEnd = ImVec2(previewLineStartX + previewLineWidth, previewLineBegin.y);
                                        ImGui::GetWindowDrawList()->AddLine(previewLineBegin, previewLineEnd, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 2);
                                    }

                                    if (dropPayload->IsDelivery() && !isSource)
                                    {
                                        if (isNodeOpen)
                                        {
                                            // If the node is open, dropping at the lower handle would be
                                            // to make the entity the first child of the dropped handle entity.
                                            setParent.HasOperation = true;
                                            setParent.SrcIndex = payloadIndex;
                                            setParent.DstParentIndex = indexInWorld;
                                            setParent.DstFlatIndexRelativeToParent = 0;
                                        }
                                        else
                                        {
                                            bool const hasParent = parentGUID != (DYE::GUID) 0;
                                            auto tryGetParentEntity = entity.GetWorld().TryGetEntityWithGUID(parentGUID);

                                            if (hasParent && tryGetParentEntity.has_value())
                                            {
                                                int parentIndex = entity.GetWorld().TryGetEntityIndex(tryGetParentEntity.value()).value();
                                                setParent.HasOperation = true;
                                                setParent.SrcIndex = payloadIndex;
                                                setParent.DstParentIndex = parentIndex;
                                                setParent.DstFlatIndexRelativeToParent = indexInWorld - parentIndex + 1;
                                            }
                                            else
                                            {
                                                reorderAtTop.HasOperation = true;
                                                reorderAtTop.SrcIndex = payloadIndex;
                                                reorderAtTop.DstIndex = indexInWorld + EntityUtil::GetEntityAndAllChildrenPreorder(entity).size();
                                            }
                                        }
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

        for (auto level: levelStack)
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

        int moveDiff = ((int) setParent.DstParentIndex - (int) setParent.SrcIndex);

        if (setParent.HasOperation)
        {
            Entity srcEntity = scene.World.GetEntityAtIndex(setParent.SrcIndex);
            Entity dstEntity = scene.World.GetEntityAtIndex(setParent.DstParentIndex);

            auto tryGetParentChildren = dstEntity.TryGetComponent<ChildrenComponent>();
            if (tryGetParentChildren.has_value())
            {
                int dstIndexInParent;

                bool const insertBegin = setParent.DstFlatIndexRelativeToParent == 0;
                bool const insertEnd = setParent.DstFlatIndexRelativeToParent == -1;
                if (insertBegin || insertEnd)
                {
                    dstIndexInParent = setParent.DstFlatIndexRelativeToParent;
                }
                else
                {
                    dstIndexInParent = 0;

                    // We need to calculate the index in parent if it's not the first OR the last location that
                    // we are trying to insert. Basically convert 'flat-index' to 'first-degree-child-index'.
                    std::vector<Entity> newParentAndItsChildren = EntityUtil::GetEntityAndAllChildrenPreorder(
                        dstEntity);

                    // We start from 1 because 0 is the parent.
                    for (int flatIndex = 1; flatIndex < setParent.DstFlatIndexRelativeToParent; flatIndex++)
                    {
                        auto childEntity = newParentAndItsChildren[flatIndex];
                        if (EntityUtil::IsFirstDegreeChildOf(childEntity, dstEntity))
                        {
                            dstIndexInParent++;
                        }
                    }
                }

                Undo::SetEntityParent
                    (
                        srcEntity,
                        setParent.SrcIndex,
                        dstEntity,
                        setParent.DstParentIndex,
                        dstIndexInParent
                    );
            }
            else // If the parent has no children, we simply make the entity as the first child of the parent.
            {
                Undo::SetEntityParent
                    (
                        srcEntity,
                        setParent.SrcIndex,
                        dstEntity,
                        setParent.DstParentIndex,
                        0
                    );
            }

            changed = true;
        }

        if (reorderAtTop.HasOperation && reorderAtTop.SrcIndex != reorderAtTop.DstIndex)
        {
            Entity srcEntity = scene.World.GetEntityAtIndex(reorderAtTop.SrcIndex);
            Undo::SetEntityOrderAtTopHierarchy(srcEntity, reorderAtTop.SrcIndex, reorderAtTop.DstIndex);
            changed = true;
        }

        return changed;
    }

    bool SceneEditorLayer::drawSceneSystemPanel(Scene &scene)
    {
        bool changed = false;

#pragma unroll
        for (int phaseIndex = static_cast<int>(ExecutionPhase::Initialize);
             phaseIndex <= static_cast<int>(ExecutionPhase::TearDown); phaseIndex++)
        {
            auto const phase = static_cast<ExecutionPhase>(phaseIndex);
            bool const isPhaseRunning = RuntimeState::IsPlaying() || (phase == ExecutionPhase::Render || phase == ExecutionPhase::PostRender);

            std::string const &phaseId = CastExecutionPhaseToString(phase);
            auto &systemDescriptors = scene.GetSystemDescriptorsOfPhase(phase);

            ImGui::PushID(phaseId.c_str());
            ImGui::Separator();

            bool const showSystems = ImGui::CollapsingHeader(phaseId.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
            if (!systemDescriptors.empty())
            {
                // Draw number of systems in this phase category on the header for easier read.
                std::size_t const numberOfSystems = systemDescriptors.size();
                int const numberOfExecutingSystems = std::count_if
                    (
                        std::execution::unseq, systemDescriptors.begin(), systemDescriptors.end(),
                        [](SystemDescriptor const &descriptor)
                        {
                            bool const isEnabled = descriptor.IsEnabled;
                            bool const isExecuting = RuntimeState::IsPlaying() || descriptor.Instance->ExecuteInEditMode();
                            return isEnabled && isExecuting;
                        }
                    );

                ImGui::SameLine();
                ImGui::Text("(%d/%zu)", numberOfExecutingSystems, numberOfSystems);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip("There are %d systems out of %zu executing in phase '%s'.", numberOfExecutingSystems, numberOfSystems, phaseId.c_str());
                }
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
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.000f, 0.000f, 0.000f, 1));
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

    template<typename Func>
    requires std::predicate<Func, std::string const &, SystemBase const *>
    bool SceneEditorLayer::drawSceneSystemList(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate)
    {
        bool changed = false;

        // Create a set for faster lookup of already included systems.
        std::unordered_set<std::string> alreadyIncludedSystems;
        alreadyIncludedSystems.reserve(systemDescriptors.size());
        std::for_each(systemDescriptors.begin(), systemDescriptors.end(),
                      [&alreadyIncludedSystems](SystemDescriptor const &descriptor)
                      {
                          alreadyIncludedSystems.insert(descriptor.Name);
                      });

        // Draw a 'Add System' button at the top of the inspector, and align it to the right side of the window.
        char const *addSystemPopupId = "Add System Menu Popup";
        ImVec2 const addButtonSize = ImVec2 {120, 0};
        float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY ? ImGui::GetWindowScrollbarRect(ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;
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
                for (auto const &[systemName, pSystemInstance]: TypeRegistry::GetSystemNamesAndInstances())
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
            auto &systemDescriptor = systemDescriptors[i];

            ImGui::PushID(systemDescriptor.Name.c_str());
            SystemBase *pSystemInstance = systemDescriptor.Instance;
            bool const isRecognizedSystem = pSystemInstance != nullptr;
            char const *headerText = isRecognizedSystem ? systemDescriptor.Name.c_str() : "(Unrecognized System)";

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
                changed = true;
                Undo::SetSystemIsEnabled(scene, systemDescriptor, i, isSystemEnabled);
            }

            // Draw header text (system name most likely).
            // Make the text look disabled if the system is not executing right now.
            bool const isExecuting = RuntimeState::IsPlaying() || systemDescriptor.Instance->ExecuteInEditMode();
            ImGui::BeginDisabled(!isExecuting);
            {
                ImGui::SameLine();
                ImGui::TextUnformatted(headerText);
            }
            ImGui::EndDisabled();

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
            ImGui::TextUnformatted("Select a valid entity to inspect.");
            return false;
        }

        bool isEntityChangedThisFrame = false;

        ImVec2 const addComponentButtonSize = ImVec2 {120, 0};
        float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY ? ImGui::GetWindowScrollbarRect(
            ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;

        // Draw entity's NameComponent as a InputField on the top.
        auto &nameComponent = entity.AddOrGetComponent<NameComponent>();
        ImGui::PushItemWidth(
            ImGui::GetWindowWidth() - scrollBarWidth - addComponentButtonSize.x - ImGui::GetFontSize());
        {
            bool const changedThisFrame = ImGui::InputText("##EntityNameComponent", &nameComponent.Name);
            if (ImGui::IsItemActivated())
            {
                context.IsModifyingEntityProperty = true;
                context.SerializedComponentBeforeModification =
                    SerializedObjectFactory::CreateSerializedComponentOfType(entity, NameComponentTypeName,
                                                                             TypeRegistry::GetComponentTypeDescriptor_NameComponent());
            }

            if (ImGui::IsItemDeactivated())
            {
                context.IsModifyingEntityProperty = false;
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                auto serializedNameComponentAfterModification =
                    SerializedObjectFactory::CreateSerializedComponentOfType(entity, NameComponentTypeName,
                                                                             TypeRegistry::GetComponentTypeDescriptor_NameComponent());
                Undo::RegisterComponentModification(entity, context.SerializedComponentBeforeModification,
                                                    serializedNameComponentAfterModification);
            }

            isEntityChangedThisFrame |= changedThisFrame;
        }
        ImGui::PopItemWidth();

        // Draw a 'Add Component' button at the top of the inspector, and align it to the right side of the window.
        char const *addComponentPopupId = "Add Component Menu Popup";
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

        if (mode == InspectorMode::Debug)
        {
            // We want to draw the entity identifier in debug mode.
            ImGui::Text("Entity Identifier: %u", entity.GetIdentifier());
        }

        // Draw a separator between the header of the inspector and the component list.
        ImGui::Separator();

        if (ImGui::BeginPopup(addComponentPopupId))
        {
            if (ImGui::BeginListBox("##Add Component List Box"))
            {
                for (auto const &[typeName, typeDescriptor]: componentNamesAndDescriptors)
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

                    bool const hasExplicitDisplayName = typeDescriptor.GetDisplayName != nullptr;
                    char const *displayName = hasExplicitDisplayName ? typeDescriptor.GetDisplayName() : typeName.c_str();

                    if (ImGui::Selectable(displayName))
                    {
                        // Add the component.
                        Undo::AddComponent(entity, typeName, typeDescriptor);
                        isEntityChangedThisFrame = true;
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    {
                        ImGui::SetTooltip(typeName.c_str());
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }

        // Draw rest of the registered component types that the entity has.

        std::unordered_set<std::string> drawnComponentTypeNames;

#ifdef DYE_EDITOR
        // In editor build, we want to be able to draw components in custom order first if the metadata is provided.

        auto tryGetEntityMetadata = entity.TryGetComponent<EntityEditorOnlyMetadata>();
        DYE_ASSERT_LOG_WARN(tryGetEntityMetadata.has_value(), "In editor build, an entity should always have 'EntityEditorOnlyMetadata' component.");

        EntityEditorOnlyMetadata &entityEditorOnlyMetadata = tryGetEntityMetadata.value();

        auto &successfullyDeserializedComponentNames = entityEditorOnlyMetadata.SuccessfullyDeserializedComponentNames;
        drawnComponentTypeNames.reserve(successfullyDeserializedComponentNames.size());

        for (auto i = 0; i < successfullyDeserializedComponentNames.size(); i++)
        {
            auto &deserializedTypeName = successfullyDeserializedComponentNames[i];
            auto tryGetTypeDescriptor = TypeRegistry::TryGetComponentTypeDescriptor(deserializedTypeName);
            DYE_ASSERT_LOG_WARN(tryGetTypeDescriptor.Success,
                                "The component '%s' was successfully deserialized but the type descriptor cannot be found in the TypeRegistry anymore.",
                                deserializedTypeName.c_str());

            char const *realFullTypeName = tryGetTypeDescriptor.FullTypeName;

            drawnComponentTypeNames.emplace(realFullTypeName);

            bool isRemoved = false;
            isEntityChangedThisFrame |= drawComponentInEntityInspector(context, realFullTypeName, tryGetTypeDescriptor.Descriptor, &isRemoved);

            if (context.ShouldEarlyOutIfInIteratorLoop)
            {
                return isEntityChangedThisFrame;
            }

            if (isRemoved)
            {
                // Early out if a component is removed, to avoid panic for-loop.
                return isEntityChangedThisFrame;
            }
        }
#endif

        // We draw rest of the components that aren't tracked by the entity metadata.
        for (auto i = 0; i < componentNamesAndDescriptors.size(); i++)
        {
            auto &[typeName, typeDescriptor] = componentNamesAndDescriptors[i];
            if (drawnComponentTypeNames.contains(typeName))
            {
                // The component inspector of the given type has already been drawn.
                // Skip it.
                continue;
            }

            bool isRemoved = false;
            isEntityChangedThisFrame |= drawComponentInEntityInspector(context, typeName, typeDescriptor, &isRemoved);

            if (context.ShouldEarlyOutIfInIteratorLoop)
            {
                return isEntityChangedThisFrame;
            }

            if (isRemoved)
            {
                // Early out if a component is removed, to avoid panic for-loop.
                return isEntityChangedThisFrame;
            }
        }

        auto tryGetDeserializationResult = entity.TryGetComponent<EntityDeserializationResult>();
        bool wasEntitySuccessfullyDeserialized = !tryGetDeserializationResult.has_value() || tryGetDeserializationResult.value().get().Success;
        if (wasEntitySuccessfullyDeserialized)
        {
            return isEntityChangedThisFrame;
        }

        // Finally, if the entity has EntityDeserializationResult && result.Success is false,
        // we want to draw the deserialization report in the inspector (i.e. Unrecognized components...etc).

        EntityDeserializationResult &deserializationResult = tryGetDeserializationResult.value();

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.000f, 0.000f, 0.000f, 1));
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
                        if (ImGui::Selectable("Copy Type Name"))
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
                    deserializationResult.UnrecognizedComponentTypeNames.erase(
                        deserializationResult.UnrecognizedComponentTypeNames.begin() + indexToRemove);
                    deserializationResult.UnrecognizedSerializedComponents.erase(
                        deserializationResult.UnrecognizedSerializedComponents.begin() + indexToRemove);
                }

                ImGui::EndTable();
            }
        }

        return isEntityChangedThisFrame;
    }

    bool SceneEditorLayer::drawComponentInEntityInspector(EntityInspectorContext &context,
                                                          std::string typeName,
                                                          ComponentTypeDescriptor typeDescriptor,
                                                          bool *pIsRemoved)
    {
        bool isEntityChangedThisFrame = false;

        Entity &entity = context.Entity;
        InspectorMode &mode = context.Mode;

        bool const hasExplicitDisplayName = typeDescriptor.GetDisplayName != nullptr;
        char const *displayName = hasExplicitDisplayName ? typeDescriptor.GetDisplayName() : typeName.c_str();

        if (mode == InspectorMode::Normal && !typeDescriptor.ShouldDrawInNormalInspector)
        {
            return isEntityChangedThisFrame;
        }

        DYE_ASSERT_LOG_WARN(typeDescriptor.Has != nullptr, "Missing 'Has' function for component '%s'.", typeName.c_str());
        if (!typeDescriptor.Has(entity))
        {
            return isEntityChangedThisFrame;
        }

        bool isHeaderVisible = true;
        bool showComponentInspector = true;

        ImGui::PushID(typeName.c_str());

        bool const useDefaultHeader = typeDescriptor.DrawHeader == nullptr;
        if (useDefaultHeader)
        {
            ImGuiTreeNodeFlags const flags = ImGuiTreeNodeFlags_DefaultOpen;
            showComponentInspector = ImGui::CollapsingHeader("##Header", &isHeaderVisible, flags);

            // Spacing ahead of the component name.
            float const spacing = ImGui::GetFrameHeight();
            ImGui::SameLine();
            ImGui::ItemSize(ImVec2(spacing, 0));

            // The display name of the component.
            ImGui::SameLine();

            if (mode == InspectorMode::Normal)
            {
                ImGui::TextUnformatted(displayName);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip(typeName.c_str());
                }
            }
            else
            {
                ImGui::Text("%s (%s)", displayName, typeName.c_str());
            }
        }
        else
        {
            // Use custom header drawer if provided.
            DrawComponentHeaderContext drawHeaderContext
                {
                    .DrawnComponentTypeName = typeName.c_str(),
                    .IsInDebugMode = (mode == InspectorMode::Debug),
                };

            showComponentInspector = typeDescriptor.DrawHeader(drawHeaderContext, entity, isHeaderVisible, displayName);
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
                auto serializedComponentAfterModification = SerializedObjectFactory::CreateSerializedComponentOfType(
                    entity, typeName, typeDescriptor);
                Undo::RegisterComponentModification(entity, context.SerializedComponentBeforeModification,
                                                    serializedComponentAfterModification);
            }

            isEntityChangedThisFrame |= drawHeaderContext.ComponentChanged;
        }
        ImGui::PopID();

        bool const isRemoved = !isHeaderVisible;
        if (isRemoved)
        {
            // Remove the component
            Undo::RemoveComponent(entity, typeName, typeDescriptor);
            *pIsRemoved = true;

            isEntityChangedThisFrame = true;
            return isEntityChangedThisFrame;
        }

        if (!showComponentInspector)
        {
            return isEntityChangedThisFrame;
        }

        bool const hasDrawInspectorFunction = typeDescriptor.DrawInspector != nullptr;
        if (!hasDrawInspectorFunction)
        {
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextWrapped("Missing 'DrawInspector' function for component '%s'. "
                                   "It's likely that the DrawInspectorFunction is not assigned when TypeRegistry::RegisterComponentType is called.",
                                   typeName.c_str());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }
        else
        {
            DrawComponentInspectorContext drawComponentInspectorContext;

            ImGui::PushID(typeName.c_str());
            isEntityChangedThisFrame |= typeDescriptor.DrawInspector(drawComponentInspectorContext, entity);
            ImGui::PopID();

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
                auto serializedComponentAfterModification = SerializedObjectFactory::CreateSerializedComponentOfType(
                    entity, typeName, typeDescriptor);
                Undo::RegisterComponentModification(entity, context.SerializedComponentBeforeModification,
                                                    serializedComponentAfterModification);
            }

            context.ShouldEarlyOutIfInIteratorLoop = drawComponentInspectorContext.ShouldEarlyOutIfInIteratorLoop;
        }

        ImGui::Spacing();
        return isEntityChangedThisFrame;
    }

    void SceneEditorLayer::drawOpenSceneDialogWindow(Scene &currentScene,
                                                     std::filesystem::path &currentScenePathContext,
                                                     bool *pIsSceneDirty,
                                                     bool &openLoadDialog,
                                                     bool &openSaveDialog)
    {
        static std::filesystem::path sceneFilePath = "";

        char const *loadScenePopupId = "Select a scene file (*.tscene)";
        char const *saveScenePopupId = "Save scene file as... ";

        // Draw open/load scene file path popup.
        if (openLoadDialog)
        {
            openLoadDialog = false;
            ImGuiUtil::OpenFilePathPopup(loadScenePopupId, "assets", currentScenePathContext, {".tscene"});
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
        if (openSaveDialog)
        {
            openSaveDialog = false;
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

    void SceneEditorLayer::OnEndOfFrame()
    {
        // Do scene view entity selection logic (using GPU-based mouse picking).

        if (!m_IsSceneViewDrawn)
        {
            return;
        }

        if (!m_IsSceneViewWindowHovered)
        {
            return;
        }

        if (m_CurrentlySelectedEntityGUID != (DYE::GUID) 0)
        {
            // If an entity is selected && operating selected entity gizmo,
            // we want to skip the scene view selection process.
            if (ImGuizmo::IsUsing() || ImGuizmo::IsOver())
            {
                return;
            }
        }

        glm::vec2 viewportSize = {m_SceneViewContext.ViewportBounds.Width, m_SceneViewContext.ViewportBounds.Height};

        auto [mouseXf, mouseYf] = ImGui::GetMousePos();
        mouseXf -= m_SceneViewContext.ViewportBounds.X;
        mouseYf -= m_SceneViewContext.ViewportBounds.Y;
        mouseYf = viewportSize.y - mouseYf;    // Flip y coordinate.

        int const mouseX = (int) mouseXf;
        int const mouseY = (int) mouseYf;

        bool const withinViewport = mouseXf >= 0 && mouseYf >= 0 && mouseXf < viewportSize.x && mouseYf < viewportSize.y;
        if (!withinViewport)
        {
            return;
        }

        if (INPUT.GetMouseButtonDown(DYE::MouseButton::Left))
        {
            // Read the entity id from the entity id framebuffer.
            int pixelValue = m_SceneViewEntityIDFramebuffer->ReadPixelAsInteger(0, mouseX, mouseY);
            if (pixelValue == -1)
            {
                // -1 means no entity is drawn at the mouse location.
                // Select nothing.
                m_CurrentlySelectedEntityGUID = (DYE::GUID) 0;
                return;
            }

            Scene &activeScene = RuntimeSceneManagement::GetActiveMainScene();
            Entity selectedEntity = activeScene.World.WrapIdentifierIntoEntity((EntityIdentifier) pixelValue);
            auto tryGetGUID = selectedEntity.TryGetGUID();

            if (!tryGetGUID.has_value())
            {
                // The entity doesn't have a GUID, we couldn't select it.
                return;
            }

            m_CurrentlySelectedEntityGUID = tryGetGUID.value();

            // Insert all parents GUID into the to-be-open set,
            // so their nodes get expanded in the hierarchy window in the next frame.
            auto tryGetParent = selectedEntity.TryGetComponent<ParentComponent>();
            while (tryGetParent.has_value())
            {
                DYE::GUID parentGUID = tryGetParent.value().get().GetParentGUID();
                m_EntityNodesToBeOpenInHierarchy.insert(parentGUID);

                selectedEntity = tryGetParent.value().get().GetParent(activeScene.World);
                tryGetParent = selectedEntity.TryGetComponent<ParentComponent>();
            }
        }
    }

    void SceneEditorLayer::OnPlayModeStateChanged(DYE::DYEditor::ModeStateChange stateChange)
    {
        auto &scene = RuntimeSceneManagement::GetActiveMainScene();
        if (stateChange == ModeStateChange::BeforeEnterPlayMode)
        {
            // Open sub-windows based on runtime configuration.
            // We don't need to check whether sub-windows are already setup OR should be setup because
            // the function will check that for us.
            SetupSubWindowsBasedOnRuntimeConfig();

            // Save a copy of the active scene as a serialized scene.
            m_SerializedSceneCacheWhenEnterPlayMode = SerializedObjectFactory::CreateSerializedScene(scene);

            // Initialize load systems (BeforeEnterPlayMode).
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

            // Initialize load systems.
            // We do this again because entering play mode also means we are loading the editor active scene.
            // Therefore, we want to trigger InitializeLoad with AfterLoadScene LoadType.
            scene.ForEachSystemDescriptor
                (
                    [&scene](SystemDescriptor &systemDescriptor, ExecutionPhase phase)
                    {
                        systemDescriptor.Instance->InitializeLoad(
                            scene.World,
                            InitializeLoadParameters
                                {
                                    .LoadType = InitializeLoadType::AfterLoadScene
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

            // If the editor user enables sub-windows in edit mode by default,
            // we don't close the sub-windows!
            auto showSubWindowsInEditMode = GetEditorConfig().GetOrDefault(EditorConfigKeys::ShowSubWindowsInEditMode, false);
            if (!showSubWindowsInEditMode)
            {
                // Close sub-windows based on runtime configuration.
                ClearSubWindowsBasedOnRuntimeConfig();
            }

            // Reapply the serialized scene back to the active scene.
            // TODO: maybe have an option to keep the changes in play mode?
            scene.Clear();
            SerializedObjectFactory::ApplySerializedSceneToEmptyScene(m_SerializedSceneCacheWhenEnterPlayMode, scene);
        }
    }


    void SceneEditorLayer::FocusSceneViewCameraToEntity(Entity entity)
    {
        glm::vec3 entityPosition = glm::vec3 {0};
        auto tryGetLocalToWorld = entity.TryGetComponent<LocalToWorldComponent>();
        if (tryGetLocalToWorld.has_value())
        {
            entityPosition = tryGetLocalToWorld.value().get().GetPosition();
        }

        glm::vec3 cameraPosition = m_SceneViewCamera.GetPosition();
        cameraPosition.x = entityPosition.x;
        cameraPosition.y = entityPosition.y;
        m_SceneViewCamera.SetPosition(cameraPosition);
    }

    void SceneEditorLayer::initializeNewSceneWithDefaultEntityAndSystems(Scene &newScene)
    {
        auto cameraEntity = newScene.World.CreateEntity("Camera");
        cameraEntity.AddComponent<LocalTransformComponent>().Position = {0, 0, 10};
        cameraEntity.AddComponent<CameraComponent>();
        cameraEntity.AddComponent<LocalToWorldComponent>();

        auto squareEntity = newScene.World.CreateEntity("Square");
        squareEntity.AddComponent<LocalTransformComponent>().Position = {0, 0, 0};
        squareEntity.AddComponent<LocalToWorldComponent>();
        squareEntity.AddComponent<SpriteRendererComponent>();

        newScene.TryAddSystemByName(ComputeLocalToWorldSystem::TypeName);
        newScene.TryAddSystemByName(RegisterCameraSystem::TypeName);
        newScene.TryAddSystemByName(Render2DSpriteSystem::TypeName);
        newScene.TryAddSystemByName(AudioSystem::TypeName);
        newScene.TryAddSystemByName(PlayAudioSourceOnInitializeSystem::TypeName);
        newScene.TryAddSystemByName(CreateWindowOnInitializeSystem::TypeName);
        newScene.TryAddSystemByName(ModifyWindowSystem::TypeName);
        newScene.TryAddSystemByName(CloseWindowOnTearDownSystem::TypeName);
    }
}