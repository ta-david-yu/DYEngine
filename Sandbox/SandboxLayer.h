#pragma once

#include "LayerBase.h"
#include "Events/KeyEvent.h"

namespace DYE
{
    class WindowBase;

    class SandboxLayer : public LayerBase
    {
    public:
        SandboxLayer(WindowBase* pWindow);
        SandboxLayer() = delete;

        void OnEvent(const std::shared_ptr<Event> &pEvent) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
        void OnImGui() override;
    private:
        bool handleOnKeyDown(const KeyDownEvent& event);
        bool handleOnKeyUp(const KeyUpEvent& event);

        WindowBase* m_pWindow;
        double m_FpsAccumulator = 0;
        int m_FramesCounter = 0;
        int m_FixedUpdateCounter = 0;
    };
}