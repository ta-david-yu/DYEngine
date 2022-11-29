#include "Base.h"
#include "SandboxLayer.h"
#include "Time.h"
#include "WindowBase.h"
#include "Logger.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/OpenGL.h"

#include <imgui.h>

namespace DYE
{
    SandboxLayer::SandboxLayer(WindowBase *pWindow) : m_pWindow(pWindow)
    {
		RenderCommand::SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		// Create vertices [position, texCoord]
		//				   [       3,        2] = 5 elements per vertex
		float vertices[5 * 4] =
			{
				-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

				0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, 0.0f, 0.0f, 1.0f
			};

		std::uint32_t indices[] =
			{
				0, 1, 2,
				2, 3, 0,
			};

		m_VertexArrayObject = VertexArray::Create();
		{
			auto vertexBufferObject = VertexBuffer::Create(vertices, sizeof(vertices));
			BufferLayout vertexLayout
				{
					BufferElement(ShaderDataType::Float3, "position", false),
					BufferElement(ShaderDataType::Float2, "texCoord", false),
				};
			vertexBufferObject->SetLayout(vertexLayout);
			m_VertexArrayObject->AddVertexBuffer(vertexBufferObject);

			auto indexBufferObject = IndexBuffer::Create(indices, sizeof(indices));
			m_VertexArrayObject->SetIndexBuffer(indexBufferObject);
		}

		m_ShaderProgram = ShaderProgram::CreateFromFile("Unlit", "assets/default/Unlit.shader");

		//m_ShaderProgram->Bind();

    }

	void SandboxLayer::OnRender()
	{
		m_ShaderProgram->Bind();

		{
			// Binding uniform variables values, ideally we want to add a data layer to this (i.e. Material data).
			// With Material class implemented, we could then have a function called RenderCommand::DrawIndexedWithMaterial()
			auto colorUniformLocation = glGetUniformLocation(m_ShaderProgram->GetID(), "_Color");
			glCall(glUniform4f(colorUniformLocation, 1, 0, 0, 1));
		}

		m_VertexArrayObject->Bind();
		RenderCommand::DrawIndexed(m_VertexArrayObject);
		m_ShaderProgram->Unbind();
		m_VertexArrayObject->Unbind();
	}

    void SandboxLayer::OnEvent(Event& event)
    {
        auto eventType = event.GetEventType();

        if (eventType == EventType::KeyDown)
        {
            auto keyEvent = static_cast<KeyDownEvent&>(event);
            DYE_ASSERT(keyEvent.GetKeyCode() != KeyCode::Space);
            DYE_ASSERT_RELEASE(keyEvent.GetKeyCode() != KeyCode::Right);

            //Logger::Log("Sandbox, KeyDown - %d", event.GetKeyCode());
            DYE_LOG("Sandbox, KeyDown - %d", keyEvent.GetKeyCode());
        }
        else if (eventType == EventType::KeyUp)
        {
            auto keyEvent = static_cast<KeyUpEvent&>(event);
            //Logger::Log("Sandbox, KeyUp - %d", event.GetKeyCode());
            DYE_LOG("Sandbox, KeyUp - %d", keyEvent.GetKeyCode());
        }
    }

    void SandboxLayer::OnUpdate()
    {
        // FPS
        m_FramesCounter++;
        m_FpsAccumulator += TIME.DeltaTime();
        if (m_FpsAccumulator >= 0.25)
        {
            double fps = m_FramesCounter / m_FpsAccumulator;
            //SDL_Log("Delta FPS: %f", fps);

            m_FramesCounter = 0;
            m_FpsAccumulator = 0;
        }
    }

    void SandboxLayer::OnFixedUpdate()
    {
        m_FixedUpdateCounter++;
        //SDL_Log("FixedDeltaTime - %f", TIME.FixedDeltaTime());
    }

    void SandboxLayer::OnImGui()
    {
        static int counter = 0;
        // get the window size as a base for calculating widgets geometry
        int sdl_width = 0, sdl_height = 0, controls_width = 0;
        SDL_GetWindowSize(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>(), &sdl_width, &sdl_height);
        controls_width = sdl_width;
        // make controls widget width to be 1/3 of the main window width
        if ((controls_width /= 3) < 300) { controls_width = 300; }

        // position the controls widget in the top-right corner with some margin
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        // here we set the calculated width and also make the height to be
        // be the height of the main window also with some margin
        ImGui::SetNextWindowSize(
                ImVec2(static_cast<float>(controls_width), static_cast<float>(sdl_height * 0.5f)),
                ImGuiCond_Always
        );

        // create a window and append into it
        ImGui::Begin("Controls", &m_IsControlWindowOpen, ImGuiWindowFlags_None);
        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
        ImGui::Text("%s", SDL_GetPlatform());
        ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
        ImGui::Text("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);

        // buttons and most other widgets return true when clicked/edited/activated
        if (ImGui::Button("Counter button"))
        {
            SDL_Log("Counter Button Clicked");
            counter++;
        }

        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("expected FPS: [%d]", TIME.FixedFramePerSecond());
        ImGui::Text("DeltaTime: [%f]", TIME.DeltaTime());
        ImGui::Text("FixedDeltaTime: [%f]", TIME.FixedDeltaTime());
        ImGui::Text("FixedUpdateFrameCounter: [%d]", m_FixedUpdateCounter);

        if (ImGui::Button("Reset FixedUpdate Counter"))
        {
            //SDL_Log("Reset FixedUpdate Counter");
            DYE_ASSERT(m_FixedUpdateCounter % 2 == 0);
            DYE_LOG_INFO("Reset Fixed Update Counter %d", m_FixedUpdateCounter);
            m_FixedUpdateCounter = 0;
        }

        if (ImGui::Button("Test Message Box Button"))
        {
            DYE_MSG_BOX(SDL_MESSAGEBOX_WARNING, "HAHA", "Test");
        }

		if (ImGui::Button("Wireframe Rendering Mode"))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		ImGui::SameLine();

		if (ImGui::Button("Normal Rendering Mode"))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

        ImGui::End();

        ImGui::ShowDemoWindow();

    }
}