#include "EditorSystem.h"

#include "World.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	void SystemBase::DrawInspector(DYE::DYEntity::World &world)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("You could override SystemBase::DrawInspector in your system class to draw anything here!");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}