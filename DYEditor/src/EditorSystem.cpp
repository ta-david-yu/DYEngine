#include "EditorSystem.h"

#include "World.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	void SystemBase::DrawInspector(DYE::DYEntity::World &world)
	{
		ImGuiUtil::DrawHelpMarker("You could override SystemBase::DrawInspector in your system class to draw anything here!");
	}
}