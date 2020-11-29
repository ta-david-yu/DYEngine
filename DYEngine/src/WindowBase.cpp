#include "WindowBase.h"
#include "SDLWindow.h"

namespace DYE
{
    std::unique_ptr<WindowBase> WindowBase::Create(const WindowProperty &windowProperty)
    {
        // TODO: add other window types. For instance, GLFWWindow
        return std::make_unique<SDLWindow>(windowProperty);
    }
}
