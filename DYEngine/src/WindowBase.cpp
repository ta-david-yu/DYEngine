#include "WindowBase.h"
#include "SDLWindow.h"

namespace DYE
{
    std::unique_ptr<WindowBase> WindowBase::Create(const WindowProperty &windowProperty)
    {
        // TODO: add other window types. For instance, GLFWWindow
        return std::make_unique<SDLWindow>(windowProperty);
    }

	std::shared_ptr<ContextBase> WindowBase::GetContext() const
	{
		return m_Context;
	}

	void WindowBase::SetContext(std::shared_ptr<ContextBase> context)
	{
		m_Context = std::move(context);
	}

	void WindowBase::MakeCurrent()
	{
		m_Context->MakeCurrentForWindow(*this);
	}

	glm::vec<2, std::int32_t> WindowBase::SetWindowSizeUsingWindowCenterAsAnchor(std::uint32_t width, std::uint32_t height)
	{
		int const previousWidth = GetWidth();
		int const previousHeight = GetHeight();
		glm::vec2 previousWindowPos = GetPosition();

		SetWindowSize(width, height);
		previousWindowPos.x -= ((int) width - previousWidth) * 0.5f;
		previousWindowPos.y -= ((int) height - previousHeight) * 0.5f;

		previousWindowPos = glm::round(previousWindowPos);
		SetWindowPosition(previousWindowPos.x, previousWindowPos.y);

		return previousWindowPos;
	}
}
