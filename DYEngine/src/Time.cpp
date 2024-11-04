#include "Core/Time.h"

#include <SDL.h>

namespace DYE
{
    Time Time::s_Instance {};

    void Time::InitSingleton(uint32_t fixedFps)
    {
        s_Instance = Time(fixedFps);
    }

    void Time::tickInit()
    {
        m_TicksSinceStart = 0;
        m_LastTicks = SDL_GetTicks();
    }

    void Time::tickUpdate()
    {
        auto now = SDL_GetTicks();

        m_DeltaTicks = now - m_LastTicks;
        m_TicksSinceStart += m_DeltaTicks;

        m_LastTicks = now;
    }
}