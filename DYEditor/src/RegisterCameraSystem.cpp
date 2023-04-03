#include "Systems/RegisterCameraSystem.h"

#include "Graphics/Camera.h"
#include "TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Graphics/RenderPipelineManager.h"
#include "ImGui/ImGuiUtil.h"

#include "Graphics/WindowManager.h"

#include <string>

namespace DYE::DYEditor
{
	void RegisterCameraSystem::Execute(DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
	{
		m_NumberOfRegisteredCamerasLastFrame = 0;
		auto& registry = DYEntity::GetWorldUnderlyingRegistry(world);
		auto view = registry.view<CameraComponent, DYEntity::TransformComponent>();
		for (auto entity : view)
		{
			auto [camera, transform] = view.get<CameraComponent, DYEntity::TransformComponent>(entity);

			Camera cameraToRegister;
			cameraToRegister.Position = transform.Position;
			cameraToRegister.Properties = camera.Properties;

			// TODO: remove this one
			cameraToRegister.Properties.TargetWindowID = WindowManager::GetMainWindow()->GetWindowID();

			RenderPipelineManager::RegisterCameraForNextRender(cameraToRegister);

			m_NumberOfRegisteredCamerasLastFrame++;
		}
	}

	void RegisterCameraSystem::DrawInspector(DYEntity::World &world)
	{
		ImGuiUtil::DrawReadOnlyTextWithLabel("Number Of Rendered Cameras Last Frame", std::to_string(m_NumberOfRegisteredCamerasLastFrame));
	}
}
