#include "SDLWindow.h"


DYE::SDLWindow::SDLWindow(const DYE::WindowProperty &windowProperty)
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

DYE::SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(m_Window);
}

void DYE::SDLWindow::OnUpdate()
{
    // TODO: Update stuff in SDLWindow, refresh frame buffer and shit
}
