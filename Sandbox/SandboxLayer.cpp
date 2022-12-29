#include "Base.h"

#include "SandboxLayer.h"
#include "WindowBase.h"
#include "Logger.h"

#include "Util/Time.h"
#include "Util/ImGuiUtil.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Texture.h"
#include "Graphics/CameraProperties.h"
#include "Graphics/Material.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DYE
{
    SandboxLayer::SandboxLayer(WindowBase *pWindow) : m_pWindow(pWindow)
    {
		RenderCommand::GetInstance().SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		// Create vertices [position, color, texCoord]
		//				   [       3,     4,        2] = 9 elements per vertex
		float vertices[9 * 4] =
			{
				-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

				0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
			};

		std::uint32_t indices[] =
			{
				0, 1, 2,
				2, 3, 0,
			};

		m_VertexArrayObject = VertexArray::Create();
		{
			auto vertexBufferObject = VertexBuffer::Create(vertices, sizeof(vertices));
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float3, "position", false),
					VertexAttribute(VertexAttributeType::Float4, "color", false),
					VertexAttribute(VertexAttributeType::Float2, "texCoord", false),
				};
			vertexBufferObject->SetLayout(vertexLayout);
			m_VertexArrayObject->AddVertexBuffer(vertexBufferObject);

			auto indexBufferObject = IndexBuffer::Create(indices, sizeof(indices));
			m_VertexArrayObject->SetIndexBuffer(indexBufferObject);
		}

		m_ShaderProgram = ShaderProgram::CreateFromFile("Shader_SpritesDefault", "assets/default/SpritesDefault.shader");
		m_ShaderProgram->Use();

		m_ProfileObject = std::make_shared<MaterialObject>();
		m_ProfileObject->Name = "Profile";
		m_ProfileObject->Material = Material::CreateFromShader("Material_Sprite", m_ShaderProgram);
		m_ProfileObject->Material->SetFloat4("_Color", glm::vec4{1, 1, 1, 1});
		m_ProfileObject->Material->SetTexture("_MainTex", Texture2D::Create("assets\\textures\\Island.png"));

		m_WhiteObject = std::make_shared<MaterialObject>();
		m_WhiteObject->Name = "White";
		m_WhiteObject->Material = Material::CreateFromShader("Material_Sprite", m_ShaderProgram);
		m_WhiteObject->Material->SetFloat4("_Color", glm::vec4{1, 1, 1, 1});
		m_WhiteObject->Material->SetTexture("_MainTex", Texture2D::Create("assets\\textures\\Island.png"));
		// Texture2D::Create(glm::vec4{1, 1, 1, 1}, 200, 100)

		m_CameraProperties = std::make_shared<CameraProperties>();
		m_CameraProperties->AspectRatio = (float) m_pWindow->GetWidth() / (float) m_pWindow->GetHeight();
		m_CameraProperties->Position = glm::vec3 {0, 0, 3};
    }

	void SandboxLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(*m_CameraProperties);
		renderMaterialObject(*m_ProfileObject);
		renderMaterialObject(*m_WhiteObject);
	}

	void SandboxLayer::renderMaterialObject(MaterialObject &object)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, object.Position);
		modelMatrix = modelMatrix * glm::toMat4(object.Rotation);

		auto tex2D = static_cast<Texture2D*>(object.Material->GetTexture("_MainTex"));
		auto scale = object.Scale * tex2D->GetScaleFromTextureDimensions();
		modelMatrix = glm::scale(modelMatrix, scale);

		RenderPipelineManager::GetActiveRenderPipeline().Submit(m_VertexArrayObject, object.Material, modelMatrix);
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
        ImGui::SetNextWindowPos
			(
				ImVec2(10, 10),
				ImGuiCond_FirstUseEver
			);
        // here we set the calculated width and also make the height to be
        // the half height of the main window
        ImGui::SetNextWindowSize
			(
                ImVec2(static_cast<float>(controls_width), static_cast<float>(sdl_height * 0.5f)),
                ImGuiCond_FirstUseEver
			);

        // create a window and append into it
		//ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        if (ImGui::Begin("Controls", &m_IsControlWindowOpen))
		{
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

			ImGui::Separator();
			ImGuiUtil::DrawCameraPropertiesControl("Camera Properties", *m_CameraProperties);

			ImGui::Separator();
			imguiMaterialObject(*m_ProfileObject);

			ImGui::Separator();
			imguiMaterialObject(*m_WhiteObject);
		}

        ImGui::End();

        ImGui::ShowDemoWindow();
    }

	void SandboxLayer::imguiMaterialObject(MaterialObject &object)
	{
		ImGui::PushID(object.Name.c_str());

		ImGuiUtil::DrawVec3Control("Position##" + object.Name, object.Position);
		ImGuiUtil::DrawVec3Control("Scale##" + object.Name, object.Scale);

		glm::vec3 rotationInEulerAnglesDegree = glm::eulerAngles(object.Rotation);
		rotationInEulerAnglesDegree += glm::vec3(0.f);
		rotationInEulerAnglesDegree = glm::degrees(rotationInEulerAnglesDegree);
		if (ImGuiUtil::DrawVec3Control("Rotation##" + object.Name, rotationInEulerAnglesDegree))
		{
			rotationInEulerAnglesDegree.y = glm::clamp(rotationInEulerAnglesDegree.y, -90.f, 90.f);
			object.Rotation = glm::quat {glm::radians(rotationInEulerAnglesDegree)};
		}

		ImGuiUtil::DrawMaterialControl(object.Material->GetName(), *object.Material);

		ImGui::PopID();
	}
}