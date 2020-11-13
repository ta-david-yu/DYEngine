#include "WindowBase.h"

#include <memory>

#include "SDLWindow.h"

std::unique_ptr<DYE::WindowBase> DYE::WindowBase::Create(const DYE::WindowProperty &windowProperty)
{
    // TODO: add other window types. For instance, GLFWWindow
    auto windowPtr = std::make_unique<DYE::SDLWindow>(windowProperty);
    return windowPtr;
}
