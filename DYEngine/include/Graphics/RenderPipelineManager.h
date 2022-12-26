#pragma once

#include "Graphics/CameraProperties.h"
#include "Util/TypeUtil.h"

#include <utility>
#include <vector>
#include <memory>

namespace DYE
{
	class RenderPipelineBase;

	class RenderPipelineManager
	{
	private:
		static std::shared_ptr<RenderPipelineBase> s_ActiveRenderPipeline;

	public:
		static void RenderWithActivePipeline(std::vector<CameraProperties> const& cameras);

		template<TypeUtil::Derived<RenderPipelineBase> T>
		static T* GetTypedActiveRenderPipelinePtr()
		{
			return static_cast<T*>(s_ActiveRenderPipeline.get());
		}

		static std::shared_ptr<RenderPipelineBase> GetActiveRenderPipeline()
		{
			return s_ActiveRenderPipeline;
		}

		static void SetActiveRenderPipeline(std::shared_ptr<RenderPipelineBase> renderPipeline)
		{
			s_ActiveRenderPipeline = std::move(renderPipeline);
		}
	};
}
