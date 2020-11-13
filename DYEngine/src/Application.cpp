#include "Application.h"
#include "SDL.h"

namespace DYE
{
    Application::Application(const std::string &windowName)
    {
        // TODO: wrap it so SDL is abstracted
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Log("Hello World");

        m_Window = WindowBase::Create(WindowProperty(windowName));
    }

    void Application::Run()
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(static_cast<SDL_Window*>(m_Window->GetNativeWindow()), -1, SDL_RENDERER_SOFTWARE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(3000);

        SDL_Quit();
    }
}
