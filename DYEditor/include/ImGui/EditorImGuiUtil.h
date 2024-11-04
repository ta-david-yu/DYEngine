#pragma once

#include "ImGui/ImGuiUtil.h"
#include "Math/Color.h"

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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.000f, 0.000f, 0.000f, 1));
        if (ImGui::Button("Fix", ImVec2(50, 0)))
        {
            fixFunction();
            isExecuted = true;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(infoText);

        ImGui::PopID();

        return isExecuted;
    }
}