#include "Application.h"
#include "Base.h"
#include "Logger.h"

#include <glad/glad.h>
#include <SDL.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <utility>

namespace DYE
{
    Application::Application(const std::string &windowName, int fixedFramePerSecond) : m_Time(fixedFramePerSecond)
    {
        // TODO: wrap it so SDL is abstracted
        SDL_Init(SDL_INIT_VIDEO);
        DYE_LOG("--------------- Hello World ---------------");

        DYE_LOG("OS: %s", SDL_GetPlatform());
        DYE_LOG("CPU cores: %d", SDL_GetCPUCount());
        DYE_LOG("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_PROFILE_MASK,
                SDL_GL_CONTEXT_PROFILE_CORE
        );

        // GL 3.0 + GLSL 130
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        int major, minor, profile;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
        DYE_LOG("GL Version: %d.%d, profile - %d", major, minor, profile);

        // Initialize system instances
        m_Window = WindowBase::Create(WindowProperty(windowName));
        m_EventSystem = EventSystemBase::Create();
        m_ImGuiLayer = std::make_shared<ImGuiLayer>(m_Window.get());

        // Register handleOnEvent member function to the EventSystem
        m_EventSystem->SetEventHandler(DYE_BIND_EVENT_FUNCTION(Application::handleOnEvent));

        // Push ImGuiLayer as overlay
        m_LayerStack.PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        SDL_Quit();
    }

    void Application::Run()
    {
        /// TEMP
        glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        glClearColor(0, 0, 0, 0);
        /// TEMP

        m_IsRunning = true;
        m_Time.tickInit();

        double deltaTimeAccumulator = 0;
        while (m_IsRunning)
        {
            /// Poll Events
            m_EventSystem->PollEvent();

            /// Fixed Update
            deltaTimeAccumulator += m_Time.DeltaTime();
            while (deltaTimeAccumulator >= m_Time.FixedDeltaTime())
            {
                for (auto& layer : m_LayerStack)
                {
                    layer->OnFixedUpdate();
                }

                deltaTimeAccumulator -= m_Time.FixedDeltaTime();
            }

            /// Update
            for (auto& layer : m_LayerStack)
            {
                layer->OnUpdate();
            }

            /// Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            onPreRenderLayers();

            for (auto& layer : m_LayerStack)
            {
                layer->OnRender();
            }

            onPostRenderLayers();

            /// ImGui
            m_ImGuiLayer->BeginImGui();
            for (auto& layer : m_LayerStack)
            {
                layer->OnImGui();
            }
            m_ImGuiLayer->EndImGui();

            /// Swap Buffers
            m_Window->OnUpdate();

            m_Time.tickUpdate();
        }
    }


    void Application::pushLayer(std::shared_ptr<LayerBase> layer)
    {
        m_LayerStack.PushLayer(std::move(layer));
    }

    void Application::pushOverlay(std::shared_ptr<LayerBase> overlay)
    {
        m_LayerStack.PushOverlay(std::move(overlay));
    }

    bool Application::handleOnEvent(const std::shared_ptr<Event>& pEvent)
    {
        auto eventType = pEvent->GetEventType();

        EventDispatcher dispatcher(*pEvent);
        dispatcher.Dispatch<WindowCloseEvent>(DYE_BIND_EVENT_FUNCTION(handleOnWindowClose));

        // Event is passed from top to bottom layer
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
        {
            // Has been handled, break the loop
            if (pEvent->IsUsed)
                break;
            (*it)->OnEvent(pEvent);
        }

        return true;
    }

    bool Application::handleOnWindowClose(const WindowCloseEvent &event)
    {
        m_IsRunning = false;
        return true;
    }
}
