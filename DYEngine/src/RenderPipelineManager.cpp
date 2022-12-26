#include "Graphics/RenderPipelineManager.h"

#include "Graphics/RenderPipelineBase.h"

namespace DYE
{
	std::shared_ptr<RenderPipelineBase> RenderPipelineManager::s_ActiveRenderPipeline = {};

	void RenderPipelineManager::RenderWithActivePipeline(std::vector<CameraProperties> const& cameras)
	{
		if (!s_ActiveRenderPipeline)
		{
			// No render pipeline is set as the active pipeline, nothing is rendered!
			return;
		}

		s_ActiveRenderPipeline->render(cameras);
	}
}