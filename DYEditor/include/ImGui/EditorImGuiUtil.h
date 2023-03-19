#pragma once

#include <string>

namespace DYE::DYEditor
{
	enum class ExecutionPhase;
}

namespace DYE::ImGuiUtil
{
	bool DrawExecutionPhaseControl(std::string const& label, DYE::DYEditor::ExecutionPhase& phase);
}