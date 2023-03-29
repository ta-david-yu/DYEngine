#pragma once

#include "Core/LayerBase.h"

#include "TypeRegistry.h"
#include "Scene.h"

#include "World.h"
#include "Entity.h"
#include "EditorSystem.h"

#include <concepts>

namespace DYE::DYEditor
{
	class EntitySceneEditorLayer : public LayerBase
	{
	public:
		EntitySceneEditorLayer();
		~EntitySceneEditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGui() override;

	private:

		// DEBUGGING
		DYEditor::Scene m_Scene;
		DYEntity::Entity m_CurrentSelectedEntity;

		DYEntity::World m_World;
		DYEntity::Entity m_Entity;

		static bool drawSceneEntityHierarchyPanel(Scene &scene, DYEntity::Entity *pCurrentSelectedEntity);

		template<typename Func> requires std::predicate<Func, std::string const&, SystemBase const*>
		static bool drawSceneSystemListPanel(Scene &scene, std::vector<SystemDescriptor> &systemDescriptors, Func addSystemFilterPredicate);

		static bool drawEntityInspector(DYEntity::Entity &entity, std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions);
		static void drawRegisteredSystems(DYEntity::World& world);
	};
}