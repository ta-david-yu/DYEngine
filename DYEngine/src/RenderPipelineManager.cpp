#include "Graphics/RenderPipelineManager.h"

#include "Base.h"

namespace DYE
{
	std::shared_ptr<RenderPipelineBase> RenderPipelineManager::s_ActiveRenderPipeline = {};
	std::vector<CameraProperties> RenderPipelineManager::s_CameraProperties = {};

	void RenderPipelineManager::RenderWithActivePipeline()
	{
		if (!s_ActiveRenderPipeline)
		{
			// No render pipeline is set as the active pipeline, nothing is rendered!
			return;
		}

		if (s_CameraProperties.empty())
		{
			DYE_LOG("RenderPipelineManager::RenderWithActivePipeline is called, but no camera has been registered.");
		}

		s_ActiveRenderPipeline->render(s_CameraProperties);

		s_CameraProperties.clear();
	}


	RenderPipelineBase& RenderPipelineManager::GetActiveRenderPipeline()
	{
		DYE_ASSERT_RELEASE(
			s_ActiveRenderPipeline.get() != nullptr &&
			"You might have forgot to set a render pipeline by calling RenderPipelineManager::SetActiveRenderPipeline beforehand?");
		return *s_ActiveRenderPipeline;
	}

	void RenderPipelineManager::SetActiveRenderPipeline(std::shared_ptr<RenderPipelineBase> renderPipeline)
	{
		s_ActiveRenderPipeline = std::move(renderPipeline);
	}

	void RenderPipelineManager::RegisterCameraForNextRender(CameraProperties cameraProperties)
	{
		s_CameraProperties.push_back(cameraProperties);
	}
}