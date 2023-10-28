#pragma once

#include "Core/LayerBase.h"

#include "Core/RuntimeState.h"
#include "Core/Scene.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Serialization/SerializedScene.h"
#include "Type/TypeRegistry.h"
#include "Core/World.h"
#include "Core/Entity.h"
#include "Graphics/Camera.h"

#include <filesystem>
#include <memory>
#include <concepts>
#include <utility>
#include <unordered_set>

#include <ImGuizmo.h>

namespace DYE
{
	class Framebuffer;
}

namespace DYE::DYEditor
{
	class SceneRuntimeLayer;

	struct SceneViewContext
	{
		Math::Rect ViewportBounds;
		int GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		bool IsGizmoLocalSpace = true;
		bool IsTransformManipulatedByGizmo = false;
		SerializedComponent SerializedTransform;
	};

	enum class InspectorMode
	{
		Normal,
		Debug
	};

	struct EntityInspectorContext
	{
		bool IsModifyingEntityProperty = false;
		/// During an operation, it might potentially modify the component list during iteration. You can set this variable to true to indicate that.
		bool ShouldEarlyOutIfInIteratorLoop = false;
		InspectorMode Mode = InspectorMode::Normal;
		DYE::DYEditor::Entity Entity;
		SerializedComponent SerializedComponentBeforeModification = SerializedObjectFactory::CreateEmptySerializedComponent();
	};

	class SceneEditorLayer : public LayerBase, public RuntimeStateListenerBase
	{
	public:
		SceneEditorLayer();
		~SceneEditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(DYE::Event &event) override;
		void OnRender() override;
		void OnImGui() override;
		void OnEndOfFrame() override;

		// Inherit from RuntimeStateListenerBase
		void OnPlayModeStateChanged(DYE::DYEditor::ModeStateChange stateChange) override;

		void SetApplication(Application* application) { m_pApplication = application; }
		void SetRuntimeLayer(std::shared_ptr<SceneRuntimeLayer> runtimeLayer) { m_RuntimeLayer = std::move(runtimeLayer); }

		void FocusSceneViewCameraToEntity(Entity entity);

	private:
		Application* m_pApplication = nullptr;
		std::shared_ptr<SceneRuntimeLayer> m_RuntimeLayer;
		SerializedScene m_SerializedSceneCacheWhenEnterPlayMode;
		bool m_IsActiveSceneDirty = false;

		bool m_IsSceneHierarchyWindowFocused = false;
		DYE::GUID m_CurrentlySelectedEntityGUID;
		std::unordered_set<DYE::GUID> m_EntityNodesToBeOpenInHierarchy;

		std::filesystem::path m_CurrentSceneFilePath;
		bool m_OpenLoadSceneFilePopup = false;
		bool m_OpenSaveSceneFilePopup = false;

		Camera m_SceneViewCamera;
		std::shared_ptr<Framebuffer> m_SceneViewCameraTargetFramebuffer;
		std::shared_ptr<Framebuffer> m_SceneViewEntityIDFramebuffer;
		float m_CameraKeyboardMoveUnitPerSecond = 10.0f;
		float m_CameraMousePanMoveUnitPerSecond = 2.0f;
		float m_CameraOrthographicSizeZoomSpeedMultiplier = 200.0f;

		bool m_IsSceneViewDrawn = true;
		bool m_IsSceneViewWindowFocused = false;
		bool m_IsSceneViewWindowHovered = false;
		SceneViewContext m_SceneViewContext;

		InspectorMode m_InspectorMode = InspectorMode::Normal;
		EntityInspectorContext m_InspectorContext;

		static void setEditorWindowDefaultLayout(ImGuiID dockSpaceId);
		static void drawEditorWindowMenuBar(Scene &currentScene, std::filesystem::path &currentScenePathContext,
											bool *pIsSceneDirty, bool &openLoadSceneFilePathPopup,
											bool &openSaveSceneFilePathPopup);
		static bool drawSceneView(Camera &sceneViewCamera, Framebuffer &entityIDFramebuffer, Entity selectedEntity, SceneViewContext &context);
		static bool drawSceneEntityHierarchyPanel(Scene &scene, DYE::GUID *pCurrentSelectedEntityGUID, std::unordered_set<DYE::GUID> &entityNodesToBeOpen);
		static bool drawSceneSystemPanel(Scene& scene);
		template<typename Func> requires std::predicate<Func, std::string const&, SystemBase const*>
		static bool drawSceneSystemList(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate);
		static bool drawEntityInspector(EntityInspectorContext &context, std::vector<std::pair<std::string, ComponentTypeDescriptor>> componentNamesAndDescriptors);
		static bool drawComponentInEntityInspector(EntityInspectorContext &context, std::string typeName,
												   ComponentTypeDescriptor typeDescriptor, bool *pIsRemoved);

		static void drawOpenSceneDialogWindow(Scene &currentScene, std::filesystem::path &currentScenePathContext, bool *pIsSceneDirty, bool &openLoadDialog, bool &openSaveDialog);

		/// This function assumes the given scene is empty.
		static void initializeNewSceneWithDefaultEntityAndSystems(Scene &newScene);
	};
}