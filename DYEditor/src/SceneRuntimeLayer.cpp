#include "SceneRuntimeLayer.h"

#include "Core/RuntimeState.h"
#include "Core/RuntimeSceneManagement.h"

namespace DYE::DYEditor
{
	SceneRuntimeLayer::SceneRuntimeLayer() : DYE::LayerBase("Runtime")
	{
	}

	void SceneRuntimeLayer::OnPreApplicationRun()
	{
		RuntimeSceneManagement::executeSceneOperationIfAny();
	}

	void SceneRuntimeLayer::OnFixedUpdate()
	{
		if (!RuntimeState::IsPlaying())
		{
			return;
		}

		ExecuteParameters const params { .Phase = ExecutionPhase::FixedUpdate };
		for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().FixedUpdateSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
		}
	}

	void SceneRuntimeLayer::OnUpdate()
	{
		if (!RuntimeState::IsPlaying())
		{
			return;
		}

		ExecuteParameters params { .Phase = ExecutionPhase::Update };
		for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().UpdateSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
		}

		params.Phase = ExecutionPhase::LateUpdate;
		for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().LateUpdateSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
		}
	}

	void SceneRuntimeLayer::OnRender()
	{
		ExecuteParameters const params { .Mode = RuntimeState::IsPlaying() ? ExecutionMode::Play : ExecutionMode::Edit, .Phase = ExecutionPhase::Render };
		for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().RenderSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
		}
	}

	void SceneRuntimeLayer::OnPostRender()
	{
		ExecuteParameters const params { .Mode = RuntimeState::IsPlaying() ? ExecutionMode::Play : ExecutionMode::Edit, .Phase = ExecutionPhase::PostRender };
		for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().PostRenderSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
		}
	}

	void SceneRuntimeLayer::OnImGui()
	{
		if (!RuntimeState::IsPlaying())
		{
			return;
		}

		ExecuteParameters const params { .Phase = ExecutionPhase::ImGui };

		for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().ImGuiSystemDescriptors)
		{
			if (!systemDescriptor.IsEnabled)
			{
				continue;
			}

			systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
		}
	}

	void SceneRuntimeLayer::OnEndOfFrame()
	{
		if (RuntimeState::IsPlaying())
		{
			ExecuteParameters const params { .Phase = ExecutionPhase::Cleanup };

			for (auto& systemDescriptor : RuntimeSceneManagement::GetActiveMainScene().CleanupSystemDescriptors)
			{
				if (!systemDescriptor.IsEnabled)
				{
					continue;
				}

				systemDescriptor.Instance->Execute(RuntimeSceneManagement::GetActiveMainScene().World, params);
			}
		}

		RuntimeState::consumeWillChangeModeIfNeeded();
		RuntimeSceneManagement::executeSceneOperationIfAny();
	}
}