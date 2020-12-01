#include "ContextBase.h"

#include "SDLWindow.h"
#include "SDLContext.h"

namespace DYE
{
    std::unique_ptr<ContextBase> ContextBase::Create(WindowBase *pWindow)
    {
        // TODO: add other context types. For instance, GLFWContext
        return std::make_unique<SDLContext>(dynamic_cast<SDLWindow*>(pWindow));
    }
}