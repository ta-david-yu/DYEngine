#pragma once

#include "Core/LayerBase.h"

#include "Internal/TypeRegistry.h"
#include "Core/Scene.h"

#include "World.h"
#include "Entity.h"
#include "Core/EditorSystem.h"

#include <memory>
#include <concepts>

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
		void OnImGui() override;

		void SetRuntimeLayer(std::shared_ptr<SceneRuntimeLayer> runtimeLayer) { m_RuntimeLayer = runtimeLayer; }

	private:
		std::shared_ptr<SceneRuntimeLayer> m_RuntimeLayer;

		DYEntity::Entity m_CurrentlySelectedEntityInHierarchyPanel;
		static void drawMainMenuBar(Scene& currentScene);
		static bool drawSceneEntityHierarchyPanel(Scene &scene, DYEntity::Entity *pCurrentSelectedEntity);
		template<typename Func> requires std::predicate<Func, std::string const&, SystemBase const*>
		static bool drawSceneSystemListPanel(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate);
		static bool drawEntityInspector(DYEntity::Entity &entity, std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions);
		static void drawRegisteredSystems(DYEntity::World& world);
	};
}