#include "Graphics/RenderPipelineBase.h"

#include "Graphics/RenderCommand.h"
#include "WindowManager.h"

namespace DYE
{
	void RenderPipelineBase::bindCameraSettings(CameraProperties& camera)
	{
		auto targetDimension = camera.GetTargetDimension();

		camera.CachedAutomaticAspectRatio = camera.GetAutomaticAspectRatioOfDimension(targetDimension);

		Math::Rect const viewportDimension = camera.GetAbsoluteViewportOfDimension(targetDimension);
		RenderCommand::GetInstance().SetViewport(viewportDimension);
	}
}