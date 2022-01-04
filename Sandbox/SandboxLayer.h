#pragma once

#include "LayerBase.h"
#include "Event/KeyEvent.h"

namespace DYE
{
    class WindowBase;

    class SandboxLayer : public LayerBase
    {
    public:
        explicit SandboxLayer(WindowBase* pWindow);
        SandboxLayer() = delete;

        void OnEvent(Event& event) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
        void OnImGui() override;
    private:
        WindowBase* m_pWindow;
        double m_FpsAccumulator = 0;
        int m_FramesCounter = 0;
        int m_FixedUpdateCounter = 0;
    };
}