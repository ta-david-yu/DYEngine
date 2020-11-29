#include "Application.h"

#include <glad/glad.h>
#include <SDL.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include "Base.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

namespace DYE
{
    Application::Application(const std::string &windowName, int framePerSecond) : m_Time(framePerSecond)
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

        double _temp_fpsAccumulator = 0;
        int _temp_framesCounter = 0;
        double fps = 0;
        int _temp_fixedUpdateCounter = 0;
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
                fps = _temp_framesCounter / _temp_fpsAccumulator;
                SDL_Log("%f", fps);

                _temp_framesCounter = 0;
                _temp_fpsAccumulator = 0;
            }

            m_EventSystem->PollEvent();

            // Main game loop
            deltaTimeAccumulator += m_Time.DeltaTime();
            while (deltaTimeAccumulator >= m_Time.FixedDeltaTime())
            {
                _temp_fixedUpdateCounter += 1;
                // TODO: FixedUpdate

                deltaTimeAccumulator -= m_Time.FixedDeltaTime();
            }

            // TODO: Update

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

                ImGui::Text("FPS: [%f]", fps);
                ImGui::Text("DeltaTime: [%f]", m_Time.DeltaTime());
                ImGui::Text("FixedUpdateCounter: [%d]", _temp_fixedUpdateCounter);

                ImGui::End();
            }

            // rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            SDL_GL_SwapWindow(window);

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

    bool Application::handleOnEvent(const std::shared_ptr<Event>& pEvent)
    {
        auto eventType = pEvent->GetEventType();

        switch (eventType)
        {
            case EventType::WindowClose:
                m_IsRunning = false;
                return true;
            case EventType::KeyDown:
                SDL_Log("KeyDown - %d", std::static_pointer_cast<KeyDownEvent>(pEvent)->GetKeyCode());
                return true;
            case EventType::KeyUp:
                SDL_Log("KeyUp - %d", std::static_pointer_cast<KeyUpEvent>(pEvent)->GetKeyCode());
                return true;
            default:
                break;
        }
        return false;
    }
}
