#include "WindowBase.h"

#include "SDLWindow.h"

DYE::WindowBase* DYE::WindowBase::Create(const DYE::WindowProperty &windowProperty)
{
    // TODO: add other window types. For instance, GLFWWindow
    return new DYE::SDLWindow(windowProperty);
}
