#include <utility>

#include "Objects/WindowCamera.h"

#include "Graphics/WindowManager.h"
#include "Graphics/WindowBase.h"
#include "Graphics/ContextBase.h"
#include "Screen.h"
#include "Math/Math.h"
#include "Util/Logger.h"

namespace DYE::MiniGame
{
	void WindowCamera::ResetCachedPosition()
	{
		m_CachedCurrentPosition = m_pWindow->GetPosition();
	}

	void WindowCamera::AssignWindow(WindowBase &window)
	{
		if (m_pWindow != nullptr)
		{
			DYE_LOG("This window camera already has an assigned window. Skip AssignWindow call");
			return;
		}
		m_pWindow = &window;

		Camera.Properties.TargetType = RenderTargetType::Window;
		Camera.Properties.TargetWindowID = m_pWindow->GetWindowID();

		Camera.Transform.Position = {0, 0, 10};
		Camera.Properties.IsOrthographic = true;
		Camera.Properties.OrthographicSize = 10;
		Camera.Properties.UseManualAspectRatio = false;
		Camera.Properties.ManualAspectRatio = (float) 1600 / 900;
		Camera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		Camera.Properties.Viewport = { 0, 0, 1, 1 };

		ResetCachedPosition();
	}

	void WindowCamera::CreateWindow(std::shared_ptr<ContextBase> contextBase, WindowProperty const& windowProperty)
	{
		if (m_pWindow != nullptr)
		{
			DYE_LOG("This window camera already has an assigned window. Skip CreateWindow call");
			return;
		}

		m_pWindow = WindowManager::CreateWindow(windowProperty);
		m_pWindow->SetContext(std::move(contextBase));
		m_pWindow->MakeCurrent();
		ContextBase::SetVSyncCountForCurrentContext(0);

		Camera.Properties.TargetType = RenderTargetType::Window;
		Camera.Properties.TargetWindowID = m_pWindow->GetWindowID();

		Camera.Transform.Position = {0, 0, 10};
		Camera.Properties.IsOrthographic = true;
		Camera.Properties.OrthographicSize = 10;
		Camera.Properties.UseManualAspectRatio = false;
		Camera.Properties.ManualAspectRatio = (float) 1600 / 900;
		Camera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		Camera.Properties.Viewport = { 0, 0, 1, 1 };

		ResetCachedPosition();
	}

	void WindowCamera::SmoothResize(std::uint32_t width, std::uint32_t height)
	{
		if (m_pWindow == nullptr)
		{
			return;
		}

		m_IsUpdatingResizeAnimation = true;
		m_ResizeAnimationTimer = 0.0f;

		m_OriginalWindowWidth = m_pWindow->GetWidth();
		m_OriginalWindowHeight = m_pWindow->GetHeight();

		m_TargetWindowWidth = width;
		m_TargetWindowHeight = height;
	}

	void WindowCamera::Translate(glm::vec2 position)
	{
		if (m_pWindow == nullptr)
		{
			return;
		}

		m_CachedCurrentPosition += position;
		m_pWindow->SetPosition(m_CachedCurrentPosition.x, m_CachedCurrentPosition.y);
	}

	WindowCamera::AnimationUpdateResult WindowCamera::UpdateWindowResizeAnimation(float timeStep)
	{
		if (m_pWindow == nullptr)
		{
			return AnimationUpdateResult::Idle;
		}

		if (!m_IsUpdatingResizeAnimation)
		{
			return AnimationUpdateResult::Idle;
		}

		m_ResizeAnimationTimer += timeStep;

		if (m_ResizeAnimationTimer >= ResizeAnimationDuration)
		{
			// Clamp to 1.0f
			m_IsUpdatingResizeAnimation = false;
			m_ResizeAnimationTimer = ResizeAnimationDuration;
		}

		float const t = EaseOutExpo(m_ResizeAnimationTimer / ResizeAnimationDuration);
		int const width = Math::Lerp(m_OriginalWindowWidth, m_TargetWindowWidth, t);
		int const height = Math::Lerp(m_OriginalWindowHeight, m_TargetWindowHeight, t);
		m_pWindow->SetWindowSizeUsingWindowCenterAsAnchor(width, height);

		ResetCachedPosition();
		return m_IsUpdatingResizeAnimation? AnimationUpdateResult::InProgress : AnimationUpdateResult::Complete;
	}

	void WindowCamera::UpdateCameraProperties()
	{
		glm::vec2 const windowPos = m_pWindow->GetPosition();
		auto windowWidth = m_pWindow->GetWidth();
		auto windowHeight = m_pWindow->GetHeight();

		glm::vec2 normalizedWindowPos = windowPos;
		normalizedWindowPos.x += windowWidth * 0.5f;
		normalizedWindowPos.y += windowHeight * 0.5f;

		int const mainDisplayIndex = m_pWindow->GetDisplayIndex();
		std::optional<DisplayMode> const displayMode = SCREEN.GetDisplayMode(mainDisplayIndex);
		float const screenWidth = displayMode->Width;
		float const screenHeight = displayMode->Height;

		normalizedWindowPos.y = screenHeight - normalizedWindowPos.y;

		float const scalar = 0.5f; //* (320.0f / windowHeight);
		Camera.Transform.Position.x = CameraOffset.x + ((normalizedWindowPos.x - screenWidth * 0.5f) / 32.0f) * scalar;
		Camera.Transform.Position.y = CameraOffset.y + ((normalizedWindowPos.y - screenHeight * 0.5f) / 32.0f) * scalar;

		float const sizeScalar = 1.0f * (320.0f / windowHeight);
		Camera.Properties.OrthographicSize = 5 / sizeScalar;
	}
}