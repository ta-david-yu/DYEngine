#include "Application.h"
#include "Base.h"
#include "Logger.h"
#include "Graphics/Renderer.h"

#include <SDL.h>
#include <SDL_image.h>

namespace DYE
{
    Application::Application(const std::string &windowName, int fixedFramePerSecond) : m_Time(fixedFramePerSecond)
    {
        SDL_Init(0);
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        DYE_LOG("--------------- Init SDL");
        DYE_LOG("OS: %s", SDL_GetPlatform());
        DYE_LOG("CPU cores: %d", SDL_GetCPUCount());
        DYE_LOG("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);

        /// Initialize system and system instances

        // Create window and context, and then init renderer
        m_Window = WindowBase::Create(WindowProperty(windowName));

        DYE_LOG("--------------- Init Renderer");
        Renderer::Init();

        // Register handleOnEvent member function to the EventSystem
        m_EventSystem = EventSystemBase::Create();
        m_EventSystem->SetEventHandler(DYE_BIND_EVENT_FUNCTION(Application::handleOnEvent));

        // Push ImGuiLayer as overlay
        m_ImGuiLayer = std::make_shared<ImGuiLayer>(m_Window.get());
        pushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        SDL_Quit();
    }

    void Application::Run()
    {
        RenderCommand::SetViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        RenderCommand::SetClearColor(glm::vec4 {0, 0, 0, 0});

        m_IsRunning = true;
        m_Time.tickInit();

        double deltaTimeAccumulator = 0;

        /// Init layers
        for (auto& layer : m_LayerStack)
        {
            DYE_LOG("--------------- Init Layer - %s", layer->GetName().c_str());
            layer->OnInit();
        }

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
            RenderCommand::Clear();

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
        dispatcher.Dispatch<WindowSizeChangeEvent>(DYE_BIND_EVENT_FUNCTION(handleOnWindowResize));

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

    bool Application::handleOnWindowResize(const WindowSizeChangeEvent &event)
    {
        Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

        // return false because others might want to handle this event too
        return false;
    }
}
