#pragma once

#include "WindowBase.h"

class SDL_Window;

namespace DYE
{
    class SDLWindow final : public WindowBase
    {
    public:
        explicit SDLWindow(const WindowProperty &windowProperty);

        ~SDLWindow() override;

        void OnUpdate() override;

        void *GetNativeWindow() const override { return m_Window; }

    private:
        SDL_Window *m_Window;
    };
}