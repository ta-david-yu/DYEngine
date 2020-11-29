#pragma once

#include "ContextBase.h"

namespace DYE
{
    class SDLWindow;

    class SDLContext : public ContextBase
    {
    public:
        explicit SDLContext(SDLWindow* pSdlWindow);

        void Init() override;
        void SwapBuffers() override;
    };
}