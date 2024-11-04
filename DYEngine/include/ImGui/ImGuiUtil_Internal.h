#pragma once

#include "ImGui/ImGuiUtil.h"

#include <vector>

namespace DYE::ImGuiUtil::Internal
{
    void InteractableItem(const char *str_id, const ImVec2 &size_arg);

    template<typename Type, typename ControlFunc> requires std::predicate<ControlFunc, std::vector<Type> &, std::size_t>
    struct ArrayControl
    {
        ArrayControl(std::string const &label, std::vector<Type> &elements, ControlFunc func) :
            Label(label), Elements(elements), ControlFunction(func) {}

        bool Draw();

        std::string const &Label;
        std::vector<Type> &Elements;
        ControlFunc ControlFunction;
    };

    // TODO: remove explicit template instantiation, want to further use concepts to constrain the function type
    // std::predicate<Func, std::vector<Type>&, std::size_t>

    using GUIDControlFunctionType = bool(std::vector<::DYE::GUID> &, std::size_t);
    extern template
    struct ArrayControl<DYE::GUID, GUIDControlFunctionType *>;
}
