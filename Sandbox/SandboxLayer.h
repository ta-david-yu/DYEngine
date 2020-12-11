#pragma once

#include "LayerBase.h"

namespace DYE
{
    class SandboxLayer : public LayerBase
    {
        void OnEvent(const std::shared_ptr<Event> &pEvent) override
    };
}