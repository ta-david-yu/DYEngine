#pragma once

#include "Core/LayerBase.h"

#include "Core/RuntimeState.h"
#include "Core/Scene.h"
#include "Core/EditorSystem.h"
#include "Serialization/SerializedScene.h"
#include "Type/TypeRegistry.h"
#include "Core/World.h"
#include "Core/Entity.h"
#include "Graphics/Camera.h"

#include <filesystem>
#include <memory>
#include <concepts>
#include <utility>

namespace DYE
{
	class Framebuffer;
}

namespace DYE::DYEditor
{
	class SceneRuntimeLayer;

	enum class InspectorMode
	{
		Normal,
		Debug
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

		// Inherit from RuntimeStateListenerBase
		void OnPlayModeStateChanged(DYE::DYEditor::ModeStateChange stateChange) override;

		void SetApplication(Application* application) { m_pApplication = application; }
		void SetRuntimeLayer(std::shared_ptr<SceneRuntimeLayer> runtimeLayer) { m_RuntimeLayer = std::move(runtimeLayer); }

	private:
		Application* m_pApplication = nullptr;
		std::shared_ptr<SceneRuntimeLayer> m_RuntimeLayer;
		SerializedScene m_SerializedSceneCacheWhenEnterPlayMode;

		DYEditor::Entity m_CurrentlySelectedEntityInHierarchyPanel;
		std::filesystem::path m_CurrentSceneFilePath;

		Camera m_SceneViewCamera;
		std::shared_ptr<Framebuffer> m_SceneViewCameraTargetFramebuffer;
		float m_CameraKeyboardMoveUnitPerSecond = 10.0f;
		float m_CameraMousePanMoveUnitPerSecond = 2.0f;
		float m_CameraOrthographicSizeZoomSpeedMultiplier = 200.0f;

		bool m_IsSceneViewWindowFocused = false;
		bool m_IsSceneViewWindowHovered = false;

		InspectorMode m_InspectorMode = InspectorMode::Normal;

		static void setEditorWindowDefaultLayout(ImGuiID dockSpaceId);
		static void drawEditorWindowMenuBar(Scene &currentScene, std::filesystem::path &currentScenePathContext);
		static void drawSceneView(Camera &sceneViewCamera);
		static bool drawSceneEntityHierarchyPanel(Scene &scene, DYEditor::Entity *pCurrentSelectedEntity);
		static bool drawSceneSystemPanel(Scene& scene);
		template<typename Func> requires std::predicate<Func, std::string const&, SystemBase const*>
		static bool drawSceneSystemList(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate);
		static bool drawEntityInspector(DYEditor::Entity &entity,
										std::vector<std::pair<std::string, ComponentTypeDescriptor>> componentNamesAndDescriptors,
										InspectorMode mode);
	};
}