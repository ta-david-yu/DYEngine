#include "Core/Application.h"
#include "Util/Macro.h"
#include "Util/Logger.h"
#include "Screen.h"
#include "Graphics/ContextBase.h"
#include "Input/InputManager.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"

#include <SDL.h>

#include <ranges>

namespace DYE
{
    Application::Application(const std::string &windowName, int fixedFramePerSecond)
    {
        SDL_Init(0);
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);			// This would implicitly init events subsystem
		SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);	// This would implicitly init joystick subsystem
		//SDL_InitSubSystem(SDL_INIT_JOYSTICK);

		SDL_GameControllerEventState(SDL_ENABLE);
		SDL_JoystickEventState(SDL_ENABLE);

		SDL_version sdlVersion;
		SDL_GetVersion(&sdlVersion);
        DYE_LOG("Init SDL: %d.%d.%d", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
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
		Screen::InitSingleton();
		InputManager::InitSingleton();

        // Create window and context
        DYE_LOG("Init Renderer");

		WindowBase* mainWindowPtr = nullptr;
		if (WindowManager::GetNumberOfWindows() == 0)
		{
			// If there is no window, we create one first as the main window!
			mainWindowPtr = WindowManager::CreateWindow(WindowProperty(windowName));
			auto context = ContextBase::Create(mainWindowPtr);
			mainWindowPtr->SetContext(context);
			mainWindowPtr->MakeCurrent();
			ContextBase::SetVSyncCountForCurrentContext(0);
		}
		else
		{
			mainWindowPtr = WindowManager::GetMainWindow();
		}

		// Initialize render pipeline
		RenderCommand::InitSingleton();
		RenderCommand::GetInstance().SetViewport(0, 0, mainWindowPtr->GetWidth(), mainWindowPtr->GetHeight());
		RenderCommand::GetInstance().SetClearColor(glm::vec4 {0, 0, 0, 0});
		RenderPipelineManager::Initialize();
		RenderPipelineManager::SetActiveRenderPipeline(std::make_shared<RenderPipeline2D>());	// Use 2D RenderPipeline by default

        // Register handleOnEvent member function to the EventSystem
        m_EventSystem = EventSystemBase::Create();
        m_EventSystem->Register(this);

        // Push ImGuiLayer as overlay, initialzied with the main window
        m_ImGuiLayer = std::make_shared<ImGuiLayer>(mainWindowPtr);
		pushOverlayImmediate(m_ImGuiLayer);
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

        // Init layers that were added before the game loop starts.
        for (auto& layer : m_LayerStack)
        {
			layer->OnPreApplicationRun();
        }

        while (m_IsRunning)
        {
            // Poll system events
            m_EventSystem->PollEvent();

			// Update input states
			INPUT.UpdateInputState();

            // Game logic fixed update
            deltaTimeAccumulator += TIME.DeltaTime();
            while (deltaTimeAccumulator >= TIME.FixedDeltaTime())
            {
                for (auto& layer : m_LayerStack)
                {
                    layer->OnFixedUpdate();
                }

                deltaTimeAccumulator -= TIME.FixedDeltaTime();
            }

            // Game logic update
            for (auto& layer : m_LayerStack)
            {
                layer->OnUpdate();
            }

            // Game logic render
			// Normally you would populate render data to the render pipeline in this phase
            onPreRenderLayers();
            for (auto& layer : m_LayerStack)
            {
                layer->OnRender();
            }
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			// Execute draw-calls on GPU
			RenderPipelineManager::RenderWithActivePipeline();

			for (auto& layer : m_LayerStack)
			{
				layer->OnPostRender();
			}


            // ImGui
            m_ImGuiLayer->BeginImGui();
            for (auto& layer : m_LayerStack)
            {
                layer->OnImGui();
            }
            m_ImGuiLayer->EndImGui();

			// Swap the buffer of the main application window
			// We swap the main window here instead of in the render pipeline manager
			// because some imgui viewports are rendered inside main window, and we want to do those first before the swap.
			// TODO: right now we call swap buffer directly because EndImGui() call already set
			//  the main window context as current. Otherwise we will have to call
			// 	mainWindow->GetContext()->MakeCurrentForWindow(mainWindow) first.
			//  At some point we want to fix this cuz it's kinda awkward and non-explicit enough
			//  and might lead to complex bugs in the future.
			RenderCommand::GetInstance().SwapWindowBuffer(*WindowManager::GetMainWindow());

            // Update all registered Windows
			// For now, it does nothing.
			WindowManager::UpdateWindows();

			// Execute delayed layer operations.
			// We do this after tickUpdate because we don't want the time spent on layer operations to be
			// taken into account for the delta time between this frame and the next frame.
			for (auto& layerOperation : m_LayerOperations)
			{
				switch (layerOperation.Type)
				{
					case ApplicationLayerOperation::OperationType::PushLayer:
						pushLayerImmediate(layerOperation.Layer);
						break;
					case ApplicationLayerOperation::OperationType::PopLayer:
						popLayerImmediate(layerOperation.Layer);
						break;
					case ApplicationLayerOperation::OperationType::PushOverlay:
						pushOverlayImmediate(layerOperation.Layer);
						break;
					case ApplicationLayerOperation::OperationType::PopOverlay:
						popOverlayImmediate(layerOperation.Layer);
						break;
				}
			}
			m_LayerOperations.clear();

			TIME.tickUpdate();
        }

        DYE_LOG("Exit Game Loop");
    }

    void Application::Handle(Event& event)
    {
        auto const& eventType = event.GetEventType();

        // Handle WindowEvent on application level
		if (eventType == EventType::ApplicationQuit)
		{
			handleOnApplicationQuit(static_cast<const ApplicationQuitEvent&>(event));
			event.IsUsed = true;
		}
		else if (eventType == EventType::WindowClose)
		{
			handleOnWindowClose(static_cast<const WindowCloseEvent&>(event));
			event.IsUsed = true;
		}

		// InputManager will handle input related event
		if (event.IsInCategory(EventCategory::Input))
		{
			INPUT.HandleSystemEvent(event);
		}

        // Event is passed from top to bottom layer
        for (auto & it : std::ranges::reverse_view(m_LayerStack))
        {
            // Has been handled, break the loop
            if (event.IsUsed)
			{
				break;
			}

            it->OnEvent(event);
        }

    }

	void Application::PushLayer(std::shared_ptr<LayerBase> &layer)
	{
		m_LayerOperations.push_back(
			ApplicationLayerOperation
				{
					.Type = ApplicationLayerOperation::OperationType::PushLayer,
					.Layer = layer
				});
	}

	void Application::PopLayer(std::shared_ptr<LayerBase> &layer)
	{
		m_LayerOperations.push_back(
			ApplicationLayerOperation
				{
					.Type = ApplicationLayerOperation::OperationType::PopLayer,
					.Layer = layer
				});
	}

	void Application::PushOverlay(std::shared_ptr<LayerBase> &overlay)
	{
		m_LayerOperations.push_back(
			ApplicationLayerOperation
				{
					.Type = ApplicationLayerOperation::OperationType::PushOverlay,
					.Layer = overlay
				});
	}

	void Application::PopOverlay(std::shared_ptr<LayerBase> &overlay)
	{
		m_LayerOperations.push_back(
			ApplicationLayerOperation
				{
					.Type = ApplicationLayerOperation::OperationType::PopOverlay,
					.Layer = overlay
				});
	}

    void Application::pushLayerImmediate(const std::shared_ptr<LayerBase> &layer)
    {
        m_LayerStack.PushLayer(layer);
    }

	void Application::popLayerImmediate(std::shared_ptr<LayerBase> layer)
	{
		m_LayerStack.PopLayer(layer);
	}

    void Application::pushOverlayImmediate(const std::shared_ptr<LayerBase>& overlay)
    {
        m_LayerStack.PushOverlay(overlay);
    }

	void Application::popOverlayImmediate(std::shared_ptr<LayerBase> overlay)
	{
		m_LayerStack.PopOverlay(overlay);
	}

	void Application::handleOnApplicationQuit(ApplicationQuitEvent const& event)
	{
		Shutdown();
	}

    void Application::handleOnWindowClose(const WindowCloseEvent &event)
    {
		auto const mainWindowID = WindowManager::TryGetMainWindowID();
		if (mainWindowID.has_value() && event.GetWindowID() == mainWindowID.value())
		{
			// If the close event comes from the main window, shutdown the application.
			Shutdown();
		}
	}

	void Application::Shutdown()
	{
		m_IsRunning = false;
	}
}
