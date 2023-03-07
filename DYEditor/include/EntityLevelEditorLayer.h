#pragma once

#include "Core/LayerBase.h"

#include "TypeRegistry.h"

#include "World.h"
#include "Entity.h"

namespace DYE::DYEditor
{
	class EntityLevelEditorLayer : public LayerBase
	{
	public:
		EntityLevelEditorLayer();
		~EntityLevelEditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGui() override;

	private:

		// DEBUGGING
		std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> m_ComponentTypeAndFunctions;
		DYEntity::World m_World;
		DYEntity::Entity m_Entity;

		static bool drawEntityInspector(DYEntity::Entity &entity, std::vector<std::pair<std::string, ComponentTypeFunctionCollection>> componentNamesAndFunctions);
	};
}