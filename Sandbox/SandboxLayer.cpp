#include "Util/Macro.h"

#include "SandboxLayer.h"
#include "Graphics/WindowBase.h"
#include "Graphics/WindowManager.h"
#include "Util/Logger.h"

#include "Util/Time.h"
#include "ImGui/ImGuiUtil.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/RenderPipelineManager.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Texture.h"
#include "Graphics/CameraProperties.h"
#include "Graphics/Material.h"

#include "Math/Color.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DYE
{
    SandboxLayer::SandboxLayer()
    {
		RenderCommand::GetInstance().SetClearColor(glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

		auto mainWindowPtr = WindowManager::GetMainWindow();

		m_CameraProperties = std::make_shared<CameraProperties>();
		m_CameraProperties->ClearColor = Color::Red;
		m_CameraProperties->TargetWindowID = mainWindowPtr->GetWindowID();
		m_CameraProperties->ManualAspectRatio = (float) mainWindowPtr->GetWidth() / (float) mainWindowPtr->GetHeight();
		m_CameraProperties->Position = glm::vec3 {0, 0, 3};
    }

	void SandboxLayer::OnRender()
	{
		RenderPipelineManager::RegisterCameraForNextRender(*m_CameraProperties);
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
    }

    void SandboxLayer::OnFixedUpdate()
    {
    }

    void SandboxLayer::OnImGui()
    {
        ImGui::ShowDemoWindow();
    }
}