#include "SceneRuntimeLayer.h"

#include "Core/ApplicationState.h"

namespace DYE::DYEditor
{
	SceneRuntimeLayer::SceneRuntimeLayer() : DYE::LayerBase("Runtime")
	{
	}

	void SceneRuntimeLayer::OnFixedUpdate()
	{
		if (!ApplicationState::IsPlaying())
		{
			return;
		}

		ExecuteParameters const params { .Phase = ExecutionPhase::FixedUpdate };
		for (auto& systemDescriptor : ActiveMainScene.FixedUpdateSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(ActiveMainScene.World, params);
		}
	}

	void SceneRuntimeLayer::OnUpdate()
	{
		if (!ApplicationState::IsPlaying())
		{
			return;
		}

		ExecuteParameters params { .Phase = ExecutionPhase::Update };
		for (auto& systemDescriptor : ActiveMainScene.UpdateSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(ActiveMainScene.World, params);
		}

		params.Phase = ExecutionPhase::LateUpdate;
		for (auto& systemDescriptor : ActiveMainScene.LateUpdateSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(ActiveMainScene.World, params);
		}
	}

	void SceneRuntimeLayer::OnRender()
	{
		ExecuteParameters const params { .Mode = ApplicationState::IsPlaying()? ExecutionMode::Play : ExecutionMode::Edit, .Phase = ExecutionPhase::Render };
		for (auto& systemDescriptor : ActiveMainScene.RenderSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(ActiveMainScene.World, params);
		}
	}

	void SceneRuntimeLayer::OnPostRender()
	{
		ExecuteParameters const params { .Mode = ApplicationState::IsPlaying()? ExecutionMode::Play : ExecutionMode::Edit, .Phase = ExecutionPhase::PostRender };
		for (auto& systemDescriptor : ActiveMainScene.PostRenderSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(ActiveMainScene.World, params);
		}
	}

	void SceneRuntimeLayer::OnImGui()
	{
		if (!ApplicationState::IsPlaying())
		{
			return;
		}

		ExecuteParameters const params { .Phase = ExecutionPhase::ImGui };

		for (auto& systemDescriptor : ActiveMainScene.ImGuiSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(ActiveMainScene.World, params);
		}
	}


}