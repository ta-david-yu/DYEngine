#include "Application.h"
#include "SDL.h"

namespace DYE
{
    Application::Application() = default;

    void Application::Run()
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Log("Hello World");

        auto window = WindowBase::Create(WindowProperty());
        m_Window = std::unique_ptr<WindowBase>(window);

        SDL_Renderer *renderer = SDL_CreateRenderer(static_cast<SDL_Window*>(window->GetNativeWindow()), -1, SDL_RENDERER_SOFTWARE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(3000);

        SDL_Quit();
    }
}
