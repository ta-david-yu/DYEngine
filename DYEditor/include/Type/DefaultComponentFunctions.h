#pragma once

#include "Core/Entity.h"
#include "ImGui/ImGuiUtil.h"
#include "Type/DrawComponentHeaderContext.h"

#include <concepts>

namespace DYE::DYEditor
{
    template<typename T>
    bool DefaultHasComponentOfType(DYE::DYEditor::Entity &entity)
    {
        return entity.HasComponent<T>();
    }

    template<typename T>
    void DefaultAddComponentOfType(DYE::DYEditor::Entity &entity)
    {
        // By default, add component to the entity using default constructor
        entity.AddComponent<T>();
    }

    template<typename T>
    void DefaultRemoveComponentOfType(DYE::DYEditor::Entity &entity)
    {
        entity.RemoveComponent<T>();
    }

    template<typename T>
    /// A concept that checks if the given type has a public member variable of type 'bool' with the name of 'IsEnabled'
    concept HasIsEnabled =
    requires(T instance)
    {
        { instance.IsEnabled } -> std::same_as<bool &>;
    };
    template<typename T>
    bool
    DefaultDrawComponentHeaderWithIsEnabled(DrawComponentHeaderContext &drawHeaderContext, DYE::DYEditor::Entity &entity, bool &isHeaderVisible, std::string const &headerLabel)
    {
        static_assert(HasIsEnabled<T>, "Type T does not have a public member variable named 'IsEnabled' of type bool.");

        ImGuiTreeNodeFlags const flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
        bool const showInspector = ImGui::CollapsingHeader("##Header", &isHeaderVisible, flags);

        ImGui::SameLine();
        drawHeaderContext.ComponentChanged |= ImGui::Checkbox("##IsEnabledCheckbox", &entity.GetComponent<T>().IsEnabled);
        drawHeaderContext.IsModificationActivated |= ImGui::IsItemActivated();
        drawHeaderContext.IsModificationDeactivated |= ImGui::IsItemDeactivated();
        drawHeaderContext.IsModificationDeactivatedAfterEdit |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SameLine();

        if (!drawHeaderContext.IsInDebugMode)
        {
            ImGui::TextUnformatted(headerLabel.c_str());
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip(drawHeaderContext.DrawnComponentTypeName);
            }
        }
        else
        {
            ImGui::Text("%s (%s)", headerLabel.c_str(), drawHeaderContext.DrawnComponentTypeName);
        }

        return showInspector;
    }
}