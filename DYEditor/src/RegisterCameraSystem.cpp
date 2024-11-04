#include "Systems/RegisterCameraSystem.h"

#include "Graphics/Camera.h"
#include "Components/TransformComponents.h"
#include "Components/CameraComponent.h"
#include "Graphics/RenderPipelineManager.h"
#include "ImGui/ImGuiUtil.h"

#include <string>

namespace DYE::DYEditor
{
    using namespace DYE::DYEditor;

    void RegisterCameraSystem::InitializeLoad(DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters)
    {
        // Call this on initialize load to perform initialization step on the group.
        auto group = world.GetRegistry().group<CameraComponent>(Get<LocalToWorldComponent>);
    }

    void RegisterCameraSystem::Execute(DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
    {
        m_NumberOfRegisteredCamerasLastFrame = 0;

        // We use group here because we know RegisterCameraSystem is the main critical path for CameraComponent.
        auto group = world.GetRegistry().group<CameraComponent>(Get<LocalToWorldComponent>);
        for (auto entity: group)
        {
            auto [camera, localToWorld] = group.get<CameraComponent, LocalToWorldComponent>(entity);

            if (!camera.IsEnabled)
            {
                continue;
            }

            Camera cameraToRegister = camera.CreateCameraWithLocalToWorldComponent(localToWorld);
            RenderPipelineManager::RegisterCameraForNextRender(cameraToRegister);

            m_NumberOfRegisteredCamerasLastFrame++;
        }
    }

    void RegisterCameraSystem::DrawInspector(DYEditor::World &world)
    {
        ImGuiUtil::DrawReadOnlyTextWithLabel("Number Of Rendered Cameras Last Frame", std::to_string(m_NumberOfRegisteredCamerasLastFrame));
    }
}
