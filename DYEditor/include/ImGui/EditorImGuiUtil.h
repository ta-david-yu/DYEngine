#pragma once

#include "ImGui/ImGuiUtil.h"

#include <string>
#include <filesystem>
#include <concepts>

namespace DYE::DYEditor
{
	enum class ExecutionPhase;
}

namespace DYE::ImGuiUtil
{
	/// Draw a line of warning info and a fix function button.
	/// \param shouldShow whether or not the info & button should be shown.
	/// \param infoText
	/// \param fixFunction a fix function that can be used to solve the warning.
	/// \return whether or not the fix function has been executed.
	template<typename Func>
	//requires std::predicate<Func>
	bool DrawTryFixWarningButtonAndInfo(
		bool shouldShow,
		char const *infoText,
		Func fixFunction)
	{
		bool isExecuted = false;

		if (!shouldShow)
		{
			return isExecuted;
		}

		ImGui::PushID(infoText);

		ImGui::Separator();

		if (ImGui::Button("Fix"))
		{
			fixFunction();
			isExecuted = true;
		}
		ImGui::SameLine();
		ImGui::TextWrapped(infoText);

		ImGui::Separator();

		ImGui::PopID();

		return isExecuted;
	}
}