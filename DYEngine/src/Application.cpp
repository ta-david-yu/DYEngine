#include "Application.h"
#include "Base.h"
#include "Logger.h"

#include <glad/glad.h>
#include <SDL.h>
#include <glm/glm.hpp>
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
/*
        SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_PROFILE_MASK,
                SDL_GL_CONTEXT_PROFILE_CORE
        );
*/

        // GL 3.0 + GLSL 130
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        int major, minor;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
        SDL_Log("GL VERSION: %d.%d", major, minor);

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

    }

    void Application::Run()
    {
        auto window = m_Window->GetTypedNativeWindowPtr<SDL_Window>();

        /// TEMP
        glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());

        ImVec4 background = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(background.x, background.y, background.z, background.w);

        {
            const char *vertShaderSrc =
                    "#version 330 core\n"
                    "\n"
                    "layout(location = 0) in vec4 position;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "   gl_Position = position;\n"
                    "}\n";
            const char *fragShaderSrc =
                    "#version 330 core\n"
                    "\n"
                    "layout(location = 0) out vec4 color;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "   color = vec4(0, 0, 0.5, 1);\n"
                    "}\n";

            unsigned int programID = glCreateProgram();

            unsigned int vsID = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vsID, 1, &vertShaderSrc, nullptr);
            glCompileShader(vsID);

            int vsCompileResult;
            glGetShaderiv(vsID, GL_COMPILE_STATUS, &vsCompileResult);
            if (vsCompileResult == GL_FALSE)
            {
                int length;
                glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &length);
                char *msg = (char *) alloca(length * sizeof(char));
                SDL_Log("Failed to compile vertex shader");
                glGetShaderInfoLog(vsID, length, &length, msg);
                SDL_Log("%s", msg);
            }

            unsigned int fsID = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fsID, 1, &fragShaderSrc, nullptr);
            glCompileShader(fsID);

            int fsCompileResult;
            glGetShaderiv(vsID, GL_COMPILE_STATUS, &fsCompileResult);
            if (fsCompileResult == GL_FALSE)
            {
                int length;
                glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &length);
                char *msg = (char *) alloca(length * sizeof(char));
                SDL_Log("Failed to compile fragment shader");
                glGetShaderInfoLog(fsID, length, &length, msg);
                SDL_Log("%s", msg);
            }

            glAttachShader(programID, vsID);
            glAttachShader(programID, fsID);
            glLinkProgram(programID);
            glValidateProgram(programID);

            glDeleteShader(vsID);
            glDeleteShader(fsID);

            glUseProgram(programID);
        }

        // Vertex Buffer
        glm::vec2 positions[3] = {
                glm::vec2 {-0.5f, -0.5f},
                glm::vec2 {0.0f, 0.5f},
                glm::vec2 {0.5f, -0.5f}
        };

        unsigned int bufferId;
        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER,  3 * sizeof(glm::vec2), positions, GL_STATIC_DRAW);

        // location (index), count (pos2d now), type (float), stride (the size of the struct), the local location pointer to the attribute (null in our case)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);


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

            // Clear
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // Draw loop
            {
                // TEMP
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }


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

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

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
