#pragma once

#include "Core/LayerBase.h"

#include "World.h"
#include "Entity.h"

namespace DYE::DYEditor
{
	class EntityLevelEditorLayer : public LayerBase
	{
	public:
		EntityLevelEditorLayer();
		~EntityLevelEditorLayer() override = default;

		void OnImGui() override;

	private:

		// DEBUGGING
		DYEntity::World m_World;
		DYEntity::Entity m_Entity;

		static bool drawEntityInspector(DYEntity::Entity& entity);
	};
}