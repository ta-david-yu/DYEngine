#pragma once

#include "Graphics/RenderPipelineBase.h"
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
		static std::vector<CameraProperties> s_CameraProperties;

	public:
		static void Initialize();

		static void RenderWithActivePipeline();

		template<class T> requires TypeUtil::DerivedFrom<T, RenderPipelineBase>
		static T* GetTypedActiveRenderPipelinePtr()
		{
			return static_cast<T*>(s_ActiveRenderPipeline.get());
		}

		static RenderPipelineBase& GetActiveRenderPipeline();
		static void SetActiveRenderPipeline(std::shared_ptr<RenderPipelineBase> renderPipeline);

		static void RegisterCameraForNextRender(CameraProperties cameraProperties);

	public:
		static bool EnableDebugDraw;
	};
}
