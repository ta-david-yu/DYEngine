#pragma once

#include "LayerBase.h"

namespace DYE
{
    class WindowBase;

    class ImGuiLayer : public LayerBase
    {
    public:
        explicit ImGuiLayer(WindowBase* pWindow);
		ImGuiLayer() = delete;
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& event) override;

        void BeginImGui();
        void EndImGui();

        inline void SetBlockEvents(bool block) { m_BlockEvents = block; }

    private:
        bool m_BlockEvents = true;
        WindowBase* m_pWindow;
    };
}