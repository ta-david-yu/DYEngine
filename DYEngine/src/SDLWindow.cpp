#include "SDLWindow.h"

#include <SDL.h>

namespace DYE
{
    SDLWindow::SDLWindow(const WindowProperty &windowProperty)
    {
        auto windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL);

        // create SDL window
        m_Window = SDL_CreateWindow(
                windowProperty.Title.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                windowProperty.Width,
                windowProperty.Height,
                windowFlags
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

    uint32_t SDLWindow::GetWidth() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_Window, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return width;
    }

    uint32_t SDLWindow::GetHeight() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_Window, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return height;
    }
}