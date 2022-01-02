#pragma once

#include "EventSystemBase.h"

namespace DYE
{
    class SDLEventSystem final : public EventSystemBase
    {
    public:
        void PollEvent() override;
        inline void SetPreProcessImGuiEvent(bool value)
        {
            m_PreProcessImGuiEvent = value;
        }

    private:
        bool m_PreProcessImGuiEvent = true;
    };
}
