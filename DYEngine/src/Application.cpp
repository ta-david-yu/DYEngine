#include "Application.h"
#include "Base.h"

#include <glad/glad.h>
#include <SDL.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

namespace DYE
{
    Application::Application(const std::string &windowName, int fixedFramePerSecond) : m_Time(fixedFramePerSecond)
    {
        // TODO: wrap it so SDL is abstracted
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Log("Hello World");

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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        // Initialize Systems
        m_Window = WindowBase::Create(WindowProperty(windowName));
        m_EventSystem = EventSystemBase::Create();

        // Register handleOnEvent member function to the EventSystem
        m_EventSystem->SetEventHandler(DYE_BIND_EVENT_FUNCTION(Application::handleOnEvent));

    }

    Application::~Application()
    {

    }

    void Application::Run()
    {
        auto window = m_Window->GetTypedNativeWindowPtr<SDL_Window>();

        /// TEMP
        glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        auto glsl_version = "#version 130";
        ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
        ImGui_ImplOpenGL3_Init(glsl_version);

        ImVec4 background = ImVec4(35/255.0f, 35/255.0f, 35/255.0f, 1.0f);
        glClearColor(background.x, background.y, background.z, background.w);

        int _temp_fixedUpdateCounter = 0;
        /// TEMP

        m_IsRunning = true;
        m_Time.tickInit();

        double deltaTimeAccumulator = 0;
        while (m_IsRunning)
        {
            m_EventSystem->PollEvent();

            // Main game loop
            deltaTimeAccumulator += m_Time.DeltaTime();
            while (deltaTimeAccumulator >= m_Time.FixedDeltaTime())
            {
                _temp_fixedUpdateCounter += 1;
                for (auto& layer : m_LayerStack)
                {
                    layer->OnFixedUpdate();
                }

                deltaTimeAccumulator -= m_Time.FixedDeltaTime();
            }

            // TODO: Update
            for (auto& layer : m_LayerStack)
            {
                layer->OnUpdate();
            }

            // TODO: Render

            /// TEMP
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // a window is defined by Begin/End pair
            {
                static int counter = 0;
                // get the window size as a base for calculating widgets geometry
                int sdl_width = 0, sdl_height = 0, controls_width = 0;
                SDL_GetWindowSize(window, &sdl_width, &sdl_height);
                controls_width = sdl_width;
                // make controls widget width to be 1/3 of the main window width
                if ((controls_width /= 3) < 300) { controls_width = 300; }

                // position the controls widget in the top-right corner with some margin
                ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
                // here we set the calculated width and also make the height to be
                // be the height of the main window also with some margin
                ImGui::SetNextWindowSize(
                        ImVec2(static_cast<float>(controls_width), static_cast<float>(sdl_height - 20)),
                        ImGuiCond_Always
                );

                // create a window and append into it
                ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize);
                ImGui::Dummy(ImVec2(0.0f, 1.0f));
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
                ImGui::Text("%s", SDL_GetPlatform());
                ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
                ImGui::Text("RAM: %.2f GB", SDL_GetSystemRAM() / 1024.0f);

                // buttons and most other widgets return true when clicked/edited/activated
                if (ImGui::Button("Counter button"))
                {
                    SDL_Log("counter button clicked");
                    counter++;
                }

                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("expected FPS: [%d]", m_Time.m_FixedFramePerSecond);
                ImGui::Text("DeltaTime: [%f]", m_Time.DeltaTime());
                ImGui::Text("FixedDeltaTime: [%f]", m_Time.FixedDeltaTime());
                ImGui::Text("FixedUpdateCounter: [%d]", _temp_fixedUpdateCounter);
                ImGui::End();
            }

            // rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // swap buffers
            m_Window->OnUpdate();

            /*
            SDL_SetRenderDrawColor(_temp_renderer, _temp_red, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(_temp_renderer);
            SDL_RenderPresent(_temp_renderer);
            */
            /// TEMP

            m_Time.tickUpdate();
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        //SDL_DestroyRenderer(_temp_renderer);
        SDL_Quit();
    }


    void Application::pushLayer(std::shared_ptr<LayerBase> layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::pushOverlay(std::shared_ptr<LayerBase> overlay)
    {
        m_LayerStack.PushOverlay(overlay);
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
            if (pEvent->IsUsed())
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
