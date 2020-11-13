#include "WindowBase.h"

#include "SDLWindow.h"

std::unique_ptr<DYE::WindowBase> DYE::WindowBase::Create(const DYE::WindowProperty &windowProperty)
{
    auto windowPtr = std::unique_ptr<WindowBase>(nullptr);

    // TODO: add other window types. For instance, GLFWWindow
    windowPtr.reset(new DYE::SDLWindow(windowProperty));
    return windowPtr;
}
