#include "Graphics/RenderPipelineManager.h"

#include <algorithm>

#include "Graphics/Camera.h"
#include "Util/Macro.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/WindowManager.h"
#include "Graphics/ContextBase.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/DebugDraw.h"

#include <optional>

namespace DYE
{
	std::shared_ptr<RenderPipelineBase> RenderPipelineManager::s_ActiveRenderPipeline = {};
	std::vector<Camera> RenderPipelineManager::s_Cameras = {};
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

		if (s_Cameras.empty())
		{
			DYE_LOG("RenderPipelineManager::RenderWithActivePipeline is called, but no camera has been registered.");
		}

		// Sort the cameras based on their render target ID.
		// to reduce the number of calls to window context swap.
		// After that, sort them based on their Depth value (from low to high).
		std::stable_sort
		(
			s_Cameras.begin(),
			s_Cameras.end(),
			[](Camera const &cameraA, Camera const &cameraB)
			{
				if (cameraA.Properties.TargetType != cameraB.Properties.TargetType)
				{
					// Two cameras render to different target type,
					// The one that targets a window goes first.
					return cameraA.Properties.TargetType == RenderTargetType::Window;
				}

				RenderTargetType const targetType = cameraA.Properties.TargetType;
				if (targetType == RenderTargetType::Window)
				{
					// In the case of both render to a window.
					if (cameraA.Properties.TargetWindowIndex != cameraB.Properties.TargetWindowIndex)
					{
						return cameraA.Properties.TargetWindowIndex < cameraB.Properties.TargetWindowIndex;
					}
				}
				else // TargetType == RenderTargetType::RenderTexture
				{
					if (cameraA.Properties.pTargetRenderTexture == nullptr)
					{
						return false;
					}

					if (cameraB.Properties.pTargetRenderTexture == nullptr)
					{
						return true;
					}

					auto const colorAttachmentA = cameraA.Properties.pTargetRenderTexture->GetColorAttachmentID(0);
					auto const colorAttachmentB = cameraB.Properties.pTargetRenderTexture->GetColorAttachmentID(0);
					if (colorAttachmentA != colorAttachmentB)
					{
						return colorAttachmentA < colorAttachmentB;
					}
				}

				// If both cameras render to the same target, always render camera that has a lower depth value,
				// so the one with higher value is rendered on top of the others.
				return cameraA.Properties.Depth <= cameraB.Properties.Depth;
			}
		);

		s_ActiveRenderPipeline->onPreRender();

		// We always render the main window first.
		// Make the main window's context current!
		WindowBase* pCurrentWindow = WindowManager::GetMainWindow();
		std::uint32_t currentWindowIndex = WindowManager::MainWindowIndex;

		Framebuffer* pCurrentFramebuffer = nullptr;
		pCurrentWindow->MakeCurrent();

		for (auto& camera : s_Cameras)
		{
			// Render to a window
			if (camera.Properties.TargetType == RenderTargetType::Window)
			{
				// Configure the new window
				if (camera.Properties.TargetWindowIndex != currentWindowIndex)
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
					pCurrentWindow = WindowManager::TryGetWindowAt(camera.Properties.TargetWindowIndex);
					if (pCurrentWindow == nullptr)
					{
						DYE_LOG("The camera render window target (index=%d) doesn't exist. Skip the camera rendering.",
								camera.Properties.TargetWindowIndex);
						continue;
					}

					currentWindowIndex = camera.Properties.TargetWindowIndex;
					pCurrentWindow->MakeCurrent();
				}
			}
			else // TargetType == RenderTargetType::RenderTexture
			{
				if (camera.Properties.pTargetRenderTexture == nullptr)
				{
					// TargetRenderTexture is not specified! Skip it.
					DYE_LOG("The camera pTargetRenderTexture is null. Skip the camera rendering.");
					continue;
				}

				if (camera.Properties.pTargetRenderTexture != pCurrentFramebuffer)
				{
					// We are done rendering the previous framebuffer.
					if (pCurrentFramebuffer != nullptr)
					{
						// Unbind the previous framebuffer.
						pCurrentFramebuffer->Unbind();
					}

					pCurrentFramebuffer = camera.Properties.pTargetRenderTexture;
					pCurrentFramebuffer->Bind();
				}
			}

			// Update camera's aspect ratio and set viewport.
			auto targetDimension = camera.Properties.GetTargetDimension();
			camera.Properties.CachedAutomaticAspectRatio = camera.Properties.GetAutomaticAspectRatioOfDimension(targetDimension);
			Math::Rect const viewportDimension = camera.Properties.GetAbsoluteViewportOfDimension(targetDimension);

			RenderCommand::GetInstance().SetViewport(viewportDimension);
			RenderCommand::GetInstance().SetClearColor(camera.Properties.ClearColor);
			RenderCommand::GetInstance().Clear();

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

		if (pCurrentFramebuffer != nullptr)
		{
			// Unbind the final rendered framebuffer.
			pCurrentFramebuffer->Unbind();
		}

		s_ActiveRenderPipeline->onPostRender();

		DebugDraw::clearDebugDraw();

		s_Cameras.clear();
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

	void RenderPipelineManager::RegisterCameraForNextRender(Camera camera)
	{
		s_Cameras.push_back(camera);
	}
}