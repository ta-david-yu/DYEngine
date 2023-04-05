#pragma once

#include "Core/LayerBase.h"

#include "Core/Scene.h"
#include "Core/EditorSystem.h"
#include "Type/TypeRegistry.h"
#include "World.h"
#include "Entity.h"
#include "Graphics/Camera.h"

#include <filesystem>
#include <memory>
#include <concepts>
#include <utility>

namespace DYE::DYEditor
{
	class SceneRuntimeLayer;

	class SceneEditorLayer : public LayerBase
	{
	public:
		SceneEditorLayer();
		~SceneEditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnRender() override;
		void OnEvent(DYE::Event &event) override;
		void OnImGui() override;

		void SetRuntimeLayer(std::shared_ptr<SceneRuntimeLayer> runtimeLayer) { m_RuntimeLayer = std::move(runtimeLayer); }

	private:
		std::shared_ptr<SceneRuntimeLayer> m_RuntimeLayer;

		DYEntity::Entity m_CurrentlySelectedEntityInHierarchyPanel;
		std::filesystem::path m_CurrentSceneFilePath;
		Camera m_SceneViewCamera;

		static void drawMainMenuBar(Scene &currentScene, std::filesystem::path &currentScenePathContext);
		static bool drawSceneEntityHierarchyPanel(Scene &scene, DYEntity::Entity *pCurrentSelectedEntity);
		template<typename Func> requires std::predicate<Func, std::string const&, SystemBase const*>
		static bool drawSceneSystemListPanel(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate);
		static bool drawEntityInspector(DYEntity::Entity &entity, std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions);
		static void drawRegisteredSystems(DYEntity::World& world);
	};
}