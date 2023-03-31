#include "Core/EditorSystem.h"

#include "Util/Macro.h"
#include "World.h"
#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	std::string CastExecutionPhaseToString(ExecutionPhase phase)
	{
		switch (phase)
		{
			case ExecutionPhase::Initialize:
				return "Initialize";
			case ExecutionPhase::FixedUpdate:
				return "FixedUpdate";
			case ExecutionPhase::Update:
				return "Update";
			case ExecutionPhase::LateUpdate:
				return "LateUpdate";
			case ExecutionPhase::Render:
				return "Render";
			case ExecutionPhase::ImGui:
				return "ImGui";
			case ExecutionPhase::Cleanup:
				return "Cleanup";
			case ExecutionPhase::TearDown:
				return "TearDown";
		}

		DYE_ASSERT(false && "Invalid Phase");
		return "Invalid Phase";
	}

	std::optional<ExecutionPhase> TryCastStringToExecutionPhase(std::string_view const& phaseInString)
	{
		if (phaseInString == "Initialize")
		{
			return ExecutionPhase::Initialize;
		}
		if (phaseInString == "FixedUpdate")
		{
			return ExecutionPhase::FixedUpdate;
		}
		if (phaseInString == "Update")
		{
			return ExecutionPhase::Update;
		}
		if (phaseInString == "LateUpdate")
		{
			return ExecutionPhase::LateUpdate;
		}
		if (phaseInString == "Render")
		{
			return ExecutionPhase::Render;
		}
		if (phaseInString == "ImGui")
		{
			return ExecutionPhase::ImGui;
		}
		if (phaseInString == "Cleanup")
		{
			return ExecutionPhase::Cleanup;
		}
		if (phaseInString == "TearDown")
		{
			return ExecutionPhase::TearDown;
		}

		DYE_ASSERT(false && "Invalid Phase String");
		return {};
	}

	void SystemBase::DrawInspector(DYE::DYEntity::World &world)
	{
		ImGuiUtil::DrawHelpMarker("You could override SystemBase::DrawInspector in your system class to draw anything here!");
	}
}