#include "SDLWindow.h"

#include <SDL.h>

namespace DYE
{
    SDLWindow::SDLWindow(const WindowProperty &windowProperty)
    {
        // create SDL window
        m_Window = SDL_CreateWindow(
                windowProperty.Title.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                windowProperty.Width,
                windowProperty.Height,
                0
        );
    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyWindow(m_Window);
    }

    void SDLWindow::OnUpdate()
    {
        // TODO: Update stuff in SDLWindow, refresh frame buffer and shit
    }
}