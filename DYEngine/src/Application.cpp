#include "Application.h"
#include "Base.h"
#include "Logger.h"
#include "Input/InputManager.h"
#include "ContextBase.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"

#include <SDL.h>

namespace DYE
{
    Application::Application(const std::string &windowName, int fixedFramePerSecond)
    {
        SDL_Init(0);
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        DYE_LOG("Init SDL");
        DYE_LOG("OS: %s", SDL_GetPlatform());
        DYE_LOG("CPU cores: %d", SDL_GetCPUCount());
        DYE_LOG("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);

        // GL 4.6 + GLSL 130
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef DYE_OPENGL_DEBUG
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

        //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        int major, minor, profile;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
        DYE_LOG("GL Version: %d.%d, profile - %s", major, minor, profile == SDL_GL_CONTEXT_PROFILE_CORE? "core" : "compatibility");

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // Initialize core systems: time, input etc
		Time::InitSingleton(fixedFramePerSecond);
		InputManager::InitSingleton();

        // Create window and context
        DYE_LOG("Init Renderer");

		if (WindowManager::GetNumberOfWindows() == 0)
		{
			// If there is not yet a window, we create one first!
			WindowManager::CreateWindow(WindowProperty(windowName));
		}

		auto mainWindow = WindowManager::GetMainWindow();
		mainWindow->GetContext().SetVSyncCount(0);

		// Initialize render pipeline
		RenderCommand::InitSingleton();
		RenderCommand::GetInstance().SetViewport(0, 0, mainWindow->GetWidth(), mainWindow->GetHeight());
		RenderCommand::GetInstance().SetClearColor(glm::vec4 {0, 0, 0, 0});
		RenderPipelineManager::SetActiveRenderPipeline(std::make_shared<RenderPipeline2D>());	// Use 2D RenderPipeline by default

        // Register handleOnEvent member function to the EventSystem
        m_EventSystem = EventSystemBase::Create();
        m_EventSystem->Register(this);

        // Push ImGuiLayer as overlay
        m_ImGuiLayer = std::make_shared<ImGuiLayer>(mainWindow);
        pushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        m_EventSystem->Unregister(this);
        SDL_Quit();
    }

    void Application::Run()
    {
        m_IsRunning = true;
        TIME.tickInit();

        double deltaTimeAccumulator = 0;

        // Init layers
        for (auto& layer : m_LayerStack)
        {
            layer->OnInit();
        }

        while (m_IsRunning)
        {
            // Poll Event
            m_EventSystem->PollEvent();

			// Update Input States
			INPUT.UpdateInputState();

            // Fixed Update
            deltaTimeAccumulator += TIME.DeltaTime();
            while (deltaTimeAccumulator >= TIME.FixedDeltaTime())
            {
                for (auto& layer : m_LayerStack)
                {
                    layer->OnFixedUpdate();
                }

                deltaTimeAccumulator -= TIME.FixedDeltaTime();
            }

            // Update
            for (auto& layer : m_LayerStack)
            {
                layer->OnUpdate();
            }

            // Render
			RenderCommand::GetInstance().Clear();

            onPreRenderLayers();
            for (auto& layer : m_LayerStack)
            {
                layer->OnRender();
            }
            onPostRenderLayers();

			RenderPipelineManager::RenderWithActivePipeline();

            // ImGui
            m_ImGuiLayer->BeginImGui();
            for (auto& layer : m_LayerStack)
            {
                layer->OnImGui();
            }
            m_ImGuiLayer->EndImGui();

            // Update all registered Windows: Swap Buffers
			WindowManager::UpdateWindows();

			TIME.tickUpdate();
        }

        DYE_LOG("Exit Game Loop");
    }

    void Application::Handle(Event& event)
    {
        auto const& eventType = event.GetEventType();
		auto const& eventCategory = event.GetCategoryFlags();

        // Handle WindowEvent on application level
        if (eventType == EventType::WindowClose)
        {
            handleOnWindowClose(static_cast<const WindowCloseEvent&>(event));
            event.IsUsed = true;
        }
        else if (eventType == EventType::WindowSizeChange)
        {
			handleOnWindowSizeChange(static_cast<const WindowSizeChangeEvent &>(event));
        }

        // Event is passed from top to bottom layer
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
        {
            // Has been handled, break the loop
            if (event.IsUsed)
			{
				break;
			}

            (*it)->OnEvent(event);
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

    void Application::handleOnWindowClose(const WindowCloseEvent &event)
    {
        m_IsRunning = false;
    }

    void Application::handleOnWindowSizeChange(const WindowSizeChangeEvent &event)
    {
		// TODO: instead of calling set viewport here,
		// TODO: set viewport depending on the window ID & whether camera is targeting the window
		// TODO: We will need something like WindowManager.GetWindowWithID(id).Resize etc
		RenderCommand::GetInstance().SetViewport(0, 0, event.GetWidth(), event.GetHeight());
    }
}
