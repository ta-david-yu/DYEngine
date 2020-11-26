#include "Application.h"

#include <SDL.h>

#include "Base.h"

namespace DYE
{
    Application::Application(const std::string &windowName, int framePerSecond) : m_Time(framePerSecond)
    {
        // TODO: wrap it so SDL is abstracted
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Log("Hello World");

        // Initialize Systems
        m_Window = WindowBase::Create(WindowProperty(windowName));
        m_EventSystem = EventSystemBase::Create();

        // Register ApplicationEvents
        // TODO: AddTypedEventListener
        m_EventSystem->AddEventListener(DYE_BIND_EVENT_FUNCTION(Application::onWindowClose));
    }

    void Application::Run()
    {
        /// TEMP
        SDL_Renderer *_temp_renderer = SDL_CreateRenderer(
                static_cast<SDL_Window *>(m_Window->GetNativeWindow()), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        uint8_t _temp_red = 0;
        double _temp_fpsAccumulator = 0;
        int _temp_framesCounter = 0;
        /// TEMP

        m_IsRunning = true;
        double deltaTimeAccumulator = 0;

        m_Time.tickInit();

        while (m_IsRunning)
        {
            // FPS
            _temp_framesCounter++;
            _temp_fpsAccumulator += m_Time.DeltaTime();
            if (_temp_fpsAccumulator >= 0.25)
            {
                double fps = _temp_framesCounter / _temp_fpsAccumulator;

                SDL_Log("Sample: [%f] seconds, FPS: [%f]", _temp_fpsAccumulator, fps);
                _temp_framesCounter = 0;
                _temp_fpsAccumulator = 0;
            }

            m_EventSystem->PollEvent();

            // Main game loop
            deltaTimeAccumulator += m_Time.DeltaTime();
            while (deltaTimeAccumulator >= m_Time.FixedDeltaTime())
            {
                _temp_red += 1;
                // TODO: FixedUpdate

                deltaTimeAccumulator -= m_Time.FixedDeltaTime();
            }

            // TODO: Update

            // TODO: Render

            /// TEMP
            SDL_SetRenderDrawColor(_temp_renderer, _temp_red, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(_temp_renderer);
            SDL_RenderPresent(_temp_renderer);
            /// TEMP

            m_Time.tickUpdate();
        }

        SDL_DestroyRenderer(_temp_renderer);
        SDL_Quit();
    }

    bool Application::onWindowClose(const std::shared_ptr<Event>& pEvent)
    {
        /// TODO: Type checking should be done in EventSystem
        if (pEvent->GetEventType() == EventType::WindowClose)
        {
            m_IsRunning = false;
            return true;
        }
        else
        {
            return false;
        }
    }
}
