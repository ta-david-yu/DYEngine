#include "SDLWindow.h"

#include <SDL.h>

namespace DYE
{
    SDLWindow::SDLWindow(const WindowProperty &windowProperty)
    {
        auto windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL);

        // create SDL window
        m_pNativeWindow = SDL_CreateWindow(
                windowProperty.Title.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                windowProperty.Width,
                windowProperty.Height,
                windowFlags
        );

        SDL_SetWindowResizable(m_pNativeWindow, (windowProperty.IsUserResizable ? SDL_TRUE : SDL_FALSE));

        // create context
        m_Context = ContextBase::Create(this);
        m_Context->Init();
    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyWindow(m_pNativeWindow);
    }

    void SDLWindow::OnUpdate()
    {
        m_Context->SwapBuffers();
    }

    uint32_t SDLWindow::GetWidth() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_pNativeWindow, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return width;
    }

    uint32_t SDLWindow::GetHeight() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_pNativeWindow, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return height;
    }

	ContextBase &SDLWindow::GetContext() const
	{
		return *m_Context;
	}
}