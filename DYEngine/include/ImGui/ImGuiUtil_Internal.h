#pragma once

#include "ImGui/ImGuiUtil.h"

#include <vector>

namespace DYE::ImGuiUtil::Internal
{
	template<typename Type, typename ControlFunc>
	struct ArrayControl
	{
		ArrayControl(std::string const& label, std::vector<Type> &elements, ControlFunc func) :
			Label(label), Elements(elements), ControlFunction(func) { }

		bool Draw();

		std::string const &Label;
		std::vector<Type> &Elements;
		ControlFunc ControlFunction;
	};

	using GUIDControlFunctionPointer = bool (*)(const char *,::DYE::GUID &);
	extern template struct ArrayControl<DYE::GUID, GUIDControlFunctionPointer>;
	//extern template bool ArrayControl<DYE::GUID, GUIDControlFunctionPointer>::Draw();

	template<typename Type, typename ControlFunc>
	bool DrawArrayOfTypeControls(const std::string& arrayLabel, std::vector<Type> &elements, ControlFunc controlFunction);
}
