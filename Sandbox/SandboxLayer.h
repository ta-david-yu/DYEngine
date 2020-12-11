#pragma once

#include "LayerBase.h"
#include "Events/KeyEvent.h"

namespace DYE
{
    class SandboxLayer : public LayerBase
    {
    public:
        void OnEvent(const std::shared_ptr<Event> &pEvent) override;
    private:
        bool handleOnKeyDown(const KeyDownEvent& event);
        bool handleOnKeyUp(const KeyUpEvent& event);
    };
}