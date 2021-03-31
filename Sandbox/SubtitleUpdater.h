#include "Scene/ComponentBase.h"

#include <imgui.h>

#include <vector>
#include <string>

namespace DYE
{
    class SubtitleUpdater : public ComponentUpdaterBase
    {
    public:
        explicit SubtitleUpdater(ComponentTypeID typeID) : ComponentUpdaterBase(typeID) { }

        std::string Description {"<empty>"};

        void UpdateComponents() override {}
        void FixedUpdateComponents() override {}

        void RemoveComponentsOfEntity(uint32_t entityID) override {}

        void OnImGui() override
        {
            ImGui::SetNextWindowPos({540, 590});
            ImGui::SetNextWindowSizeConstraints({520, 100}, {520, 100});

            if (ImGui::Begin("##subtitle", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
            {
                ImGui::SetWindowFontScale(1.2);
                ImGui::TextWrapped("%s", Description.c_str());
            }
            ImGui::End();
        }
    };
}