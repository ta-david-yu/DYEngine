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
	void WindowCamera::AssignWindow(WindowBase &window)
	{
		if (pWindow != nullptr)
		{
			DYE_LOG("This window camera already has an assigned window. Skip AssignWindow call");
			return;
		}
		pWindow = &window;

		Camera.Properties.TargetType = RenderTargetType::Window;
		Camera.Properties.TargetWindowID = pWindow->GetWindowID();

		Camera.Transform.Position = {0, 0, 10};
		Camera.Properties.IsOrthographic = true;
		Camera.Properties.OrthographicSize = 10;
		Camera.Properties.UseManualAspectRatio = false;
		Camera.Properties.ManualAspectRatio = (float) 1600 / 900;
		Camera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		Camera.Properties.Viewport = { 0, 0, 1, 1 };
	}

	void WindowCamera::CreateWindow(std::shared_ptr<ContextBase> contextBase, WindowProperty const& windowProperty)
	{
		if (pWindow != nullptr)
		{
			DYE_LOG("This window camera already has an assigned window. Skip CreateWindow call");
			return;
		}

		pWindow = WindowManager::CreateWindow(windowProperty);
		pWindow->SetContext(std::move(contextBase));
		pWindow->MakeCurrent();
		ContextBase::SetVSyncCountForCurrentContext(0);

		Camera.Properties.TargetType = RenderTargetType::Window;
		Camera.Properties.TargetWindowID = pWindow->GetWindowID();

		Camera.Transform.Position = {0, 0, 10};
		Camera.Properties.IsOrthographic = true;
		Camera.Properties.OrthographicSize = 10;
		Camera.Properties.UseManualAspectRatio = false;
		Camera.Properties.ManualAspectRatio = (float) 1600 / 900;
		Camera.Properties.ViewportValueType = ViewportValueType::RelativeDimension;
		Camera.Properties.Viewport = { 0, 0, 1, 1 };
	}

	void WindowCamera::SmoothResize(std::uint32_t width, std::uint32_t height)
	{
		if (pWindow == nullptr)
		{
			return;
		}

		m_IsUpdatingResizeAnimation = true;
		m_TargetWindowWidth = width;
		m_TargetWindowHeight = height;
	}

	void WindowCamera::SmoothMove(glm::vec<2, std::int32_t> position)
	{
		if (pWindow == nullptr)
		{
			return;
		}

		m_IsUpdatingMoveAnimation = true;
		m_CachedCurrentPosition = pWindow->GetPosition();
		m_TargetPosition = position;
	}

	WindowCamera::AnimationUpdateResult WindowCamera::UpdateWindowResizeAnimation(float timeStep)
	{
		if (pWindow == nullptr)
		{
			return AnimationUpdateResult::Idle;
		}

		if (!m_IsUpdatingResizeAnimation)
		{
			return AnimationUpdateResult::Idle;
		}

		int currWidth = pWindow->GetWidth();
		int currHeight = pWindow->GetHeight();

		int const widthDiff = m_TargetWindowWidth - currWidth;
		int const heightDiff = m_TargetWindowHeight - currHeight;
		if (glm::abs(widthDiff) == 0 && glm::abs(heightDiff) == 0)
		{
			m_IsUpdatingResizeAnimation = false;
			return AnimationUpdateResult::Complete;
		}

		// TODO: Fixed precision issue because window size is int, but animation is using float!
		//  We prolly want to use fixed animation instead (tween animation)
		float const newWidth = Math::Lerp(currWidth, m_TargetWindowWidth, 1.0f - glm::pow(0.5f, timeStep / 0.1f));
		float const newHeight = Math::Lerp(currHeight, m_TargetWindowHeight, 1.0f - glm::pow(0.5f, timeStep / 0.1f));

		// Make the speed even, so SetWindowSizeUsingWindowCenterAsAnchor can properly derive precise integer position
		int widthSpeed = glm::abs(newWidth - currWidth);
		if (widthSpeed % 2 == 1) widthSpeed += 1;

		int heightSpeed = glm::abs(newHeight - currHeight);
		if (heightSpeed % 2 == 1) heightSpeed += 1;

		currWidth += glm::sign(widthDiff) * std::min((int) widthSpeed, glm::abs(widthDiff));
		currHeight += glm::sign(heightDiff) * std::min((int) heightSpeed, glm::abs(heightDiff));
		pWindow->SetWindowSizeUsingWindowCenterAsAnchor(currWidth, currHeight);

		if (glm::abs(m_TargetWindowWidth - currWidth) < 5 && glm::abs(m_TargetWindowHeight - currHeight) < 5)
		{
			pWindow->SetWindowSizeUsingWindowCenterAsAnchor(m_TargetWindowWidth, m_TargetWindowHeight);

			m_IsUpdatingResizeAnimation = false;
			return AnimationUpdateResult::Complete;
		}

		return AnimationUpdateResult::InProgress;
	}

	WindowCamera::AnimationUpdateResult WindowCamera::UpdateWindowMoveAnimation(float timeStep)
	{
		if (pWindow == nullptr)
		{
			return AnimationUpdateResult::Idle;
		}

		if (!m_IsUpdatingMoveAnimation)
		{
			return AnimationUpdateResult::Idle;
		}

		m_CachedCurrentPosition = Math::Lerp(m_CachedCurrentPosition, m_TargetPosition, 1.0f - glm::pow(0.5f, timeStep / 0.1f));
		pWindow->SetPosition(m_CachedCurrentPosition.x, m_CachedCurrentPosition.y);

		if (glm::length2(m_CachedCurrentPosition - m_TargetPosition) <= 0.01f)
		{
			pWindow->SetPosition(m_TargetPosition.x, m_TargetPosition.y);

			m_IsUpdatingMoveAnimation = false;
			return AnimationUpdateResult::Complete;
		}

		return AnimationUpdateResult::InProgress;
	}

	void WindowCamera::UpdateCameraProperties()
	{
		glm::vec2 const windowPos = pWindow->GetPosition();
		auto windowWidth = pWindow->GetWidth();
		auto windowHeight = pWindow->GetHeight();

		glm::vec2 normalizedWindowPos = windowPos;
		normalizedWindowPos.x += windowWidth * 0.5f;
		normalizedWindowPos.y += windowHeight * 0.5f;

		int const mainDisplayIndex = pWindow->GetDisplayIndex();
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