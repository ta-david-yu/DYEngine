#include "Graphics/WindowBase.h"
#include "Graphics/SDLWindow.h"

#include "Util/Logger.h"

#include <SDL.h>

namespace DYE
{
    std::unique_ptr<WindowBase> WindowBase::Create(const WindowProperty &windowProperty)
    {
        // TODO: add other window types. For instance, GLFWWindow
        return std::make_unique<SDLWindow>(windowProperty);
    }

	WindowID WindowBase::GetMouseFocusedWindowID()
	{
		SDL_Window* pSDLWindow = SDL_GetMouseFocus();
		return SDL_GetWindowID(pSDLWindow);
	}

	std::shared_ptr<ContextBase> WindowBase::GetContext() const
	{
		if (m_Context == nullptr)
		{
			DYE_LOG_WARN("WindowBase::GetContext: m_Context has not been assigned therefore the return value is invalid."
						  "You might have forget to call SetContext() for window %d.", GetWindowID());
		}

		return m_Context;
	}

	void WindowBase::SetContext(std::shared_ptr<ContextBase> context)
	{
		m_Context = std::move(context);
	}

	void WindowBase::MakeCurrent()
	{
		if (m_Context == nullptr)
		{
			DYE_LOG_ERROR("WindowBase::MakeCurrent: Make current failed because m_Context is null. "
						  "You might have forget to call SetContext() for window %d.", GetWindowID());
		}

		m_Context->MakeCurrentForWindow(*this);
	}

	glm::vec<2, std::int32_t> WindowBase::SetWindowSizeUsingWindowCenterAsAnchor(std::uint32_t width, std::uint32_t height)
	{
		int const previousWidth = GetWidth();
		int const previousHeight = GetHeight();
		glm::vec2 previousWindowPos = GetPosition();

		SetSize(width, height);
		previousWindowPos.x -= ((int) width - previousWidth) * 0.5f;
		previousWindowPos.y -= ((int) height - previousHeight) * 0.5f;

		previousWindowPos = glm::round(previousWindowPos);
		SetPosition(previousWindowPos.x, previousWindowPos.y);

		return previousWindowPos;
	}

	void WindowBase::SetPosition(glm::vec<2, std::int32_t> position)
	{
		SetPosition(position.x, position.y);
	}
}
