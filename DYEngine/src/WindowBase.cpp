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
}
