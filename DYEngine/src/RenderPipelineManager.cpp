#include "Graphics/RenderPipelineManager.h"

#include "Util/Macro.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/WindowManager.h"
#include "Graphics/ContextBase.h"
#include "Graphics/DebugDraw.h"

#include <optional>

namespace DYE
{
	std::shared_ptr<RenderPipelineBase> RenderPipelineManager::s_ActiveRenderPipeline = {};
	std::vector<CameraProperties> RenderPipelineManager::s_CameraProperties = {};
	bool RenderPipelineManager::EnableDebugDraw = true;

	void RenderPipelineManager::Initialize()
	{
		DebugDraw::initialize();
	}

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

		// We always render the main window first.
		// Make the main window's context current!
		WindowBase* pCurrentWindow = WindowManager::GetMainWindow();
		pCurrentWindow->MakeCurrent();
		RenderCommand::GetInstance().Clear();

		for (auto& camera : s_CameraProperties)
		{
			if (camera.TargetType == RenderTargetType::RenderTexture)
			{
				// TODO: we skip render texture camera cuz we haven't implemented it yet!
				continue;
			}

			// Render to a window

			// Configure the new window
			if (camera.TargetWindowID != pCurrentWindow->GetWindowID())
			{
				// We are done rendering the previous window.
				// Swap the buffer of the previous window so the buffer is actually drawn on the screen.

				// Note that we only swap buffers of non-main windows because we want to render
				// imgui contexts to the main window later. Therefore, main window will be flushed/swapped
				// after imgui rendering.
				if (!WindowManager::IsMainWindow(*pCurrentWindow))
				{
					RenderCommand::GetInstance().SwapWindowBuffer(*pCurrentWindow);
				}

				// If the camera is rendering to a window other than the current one,
				// Swap to the render target window and make the context current.
				pCurrentWindow = WindowManager::GetWindowFromID(camera.TargetWindowID);

				if (pCurrentWindow == nullptr)
				{
					DYE_LOG("The camera render window target (%d) doesn't exist. Skip the camera rendering.", camera.TargetWindowID);
					continue;
				}

				pCurrentWindow->MakeCurrent();

				RenderCommand::GetInstance().Clear();
			}

			// Update camera's aspect ratio and set viewport.
			auto targetDimension = camera.GetTargetDimension();
			camera.CachedAutomaticAspectRatio = camera.GetAutomaticAspectRatioOfDimension(targetDimension);
			Math::Rect const viewportDimension = camera.GetAbsoluteViewportOfDimension(targetDimension);

			RenderCommand::GetInstance().SetViewport(viewportDimension);

			s_ActiveRenderPipeline->renderCamera(camera);

			// Render DebugDraw at the end to make sure debug gizmos are on top of other objects.
			DebugDraw::renderDebugDrawOnCamera(camera);
		}

		if (pCurrentWindow != nullptr && !WindowManager::IsMainWindow(*pCurrentWindow))
		{
			// Swap the final rendered window if it's not the main window.
			// Note that we only swap buffers of non-main windows because we want to render
			// imgui contexts to the main window later.
			RenderCommand::GetInstance().SwapWindowBuffer(*pCurrentWindow);
		}

		s_ActiveRenderPipeline->onPostRender();

		DebugDraw::clearDebugDraw();

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