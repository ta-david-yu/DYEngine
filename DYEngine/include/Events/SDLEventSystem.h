#pragma once

#include "EventSystemBase.h"

namespace DYE
{
    class SDLEventSystem final : public EventSystemBase
    {
    public:
        void PollEvent() override;
    };
}
