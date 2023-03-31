#include "Systems/Render2DSpriteSystem.h"

#include "ImGui/ImGuiUtil.h"

#include <string>

namespace DYE::DYEditor
{
	void Render2DSpriteSystem::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
	{
		// TODO:
	}

	void Render2DSpriteSystem::DrawInspector(DYEntity::World &world)
	{
		ImGuiUtil::DrawReadOnlyTextWithLabel("Number Of Rendered Entities This Frame", std::to_string(m_NumberOfRenderedEntitiesThisFrame));
	}
}
