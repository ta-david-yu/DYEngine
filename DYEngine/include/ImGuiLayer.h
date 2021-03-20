#pragma once

#include "LayerBase.h"

namespace DYE
{
    class WindowBase;

    class ImGuiLayer : public LayerBase
    {
    public:
        ImGuiLayer(WindowBase* pWindow);
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(const std::shared_ptr<Event> &pEvent) override;

        void BeginImGui();
        void EndImGui();

        inline void SetBlockEvents(bool block) { m_BlockEvents = block; }

    private:
        bool m_BlockEvents = true;
        WindowBase* m_pWindow;
    };
}