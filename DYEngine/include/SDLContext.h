#pragma once

#include "ContextBase.h"

namespace DYE
{
    class SDLWindow;

    /// A GraphicsContext that stores the states associated with SDL2 OpenGL
    class SDLContext : public ContextBase
    {
    public:
        explicit SDLContext(SDLWindow* pSdlWindow);
        ~SDLContext() override;

        void Init() override;
        void SwapBuffers() override;
    private:
        /// The owner of this Context
        SDLWindow* m_pWindow;

        /// The pointer to the native Context object (SDL)
        void* m_pNativeContext;
    };
}