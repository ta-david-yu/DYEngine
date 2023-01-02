#include "Graphics/RenderPipelineManager.h"

#include "Base.h"
#include "Graphics/RenderCommand.h"
#include "WindowManager.h"
#include "ContextBase.h"

#include <optional>

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

		// Sort the cameras based on their render target ID (for now only Window),
		// to reduce the number of calls to window context swap.
		std::stable_sort
			(
				s_CameraProperties.begin(),
				s_CameraProperties.end(),
				[](CameraProperties const &cameraA, CameraProperties const &cameraB)
				{
					if (cameraA.TargetType == RenderTargetType::Window && cameraB.TargetType == RenderTargetType::Window)
					{
						return cameraA.TargetWindowID < cameraB.TargetWindowID;
					}

					// If only camera A is rendering to window, we render camera A first!
					return cameraA.TargetType == RenderTargetType::Window;
				}
			);

		s_ActiveRenderPipeline->onPreRender();

		WindowBase* pCurrentWindow = nullptr;
		std::optional<WindowID> currentWindowID = {};
		for (auto& camera : s_CameraProperties)
		{
			if (camera.TargetType == RenderTargetType::RenderTexture)
			{
				// TODO: we skip render texture camera cuz we haven't implemented it yet!
				continue;
			}

			// Render to a window
			if (pCurrentWindow == nullptr || camera.TargetWindowID != pCurrentWindow->GetWindowID())
			{
				//if (pCurrentWindow != nullptr)
				{
					//pCurrentWindow->OnUpdate();
				}

				// If the camera is rendering to a window other than the current one,
				// Swap to the render target window and make the context current.
				currentWindowID = camera.TargetWindowID;
				pCurrentWindow = WindowManager::GetWindowFromID(currentWindowID.value());
				pCurrentWindow->GetContext()->MakeCurrentForWindow(pCurrentWindow);

				RenderCommand::GetInstance().Clear();
			}

			s_ActiveRenderPipeline->bindCameraSettings(camera);
			s_ActiveRenderPipeline->renderCamera(camera);
		}

		s_ActiveRenderPipeline->onPostRender();

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