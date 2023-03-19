#include "ImGui/EditorImGuiUtil.h"

#include "ImGui/ImGuiUtil.h"
#include "EditorSystem.h"

namespace DYE::ImGuiUtil
{
	bool DrawExecutionPhaseControl(const std::string &label, DYE::DYEditor::ExecutionPhase &phase)
	{
		auto phaseValue = static_cast<int32_t>(phase);
		return DrawDropdown
					(
						label,
						phaseValue,
						// TODO: The list is pending to change, see ExecutionPhase for further information.
						{ "Initialize", "Update", "FixedUpdate", "Render", "ImGui" }
					);
	}
}
