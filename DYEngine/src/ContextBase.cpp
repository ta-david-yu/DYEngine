#include "ContextBase.h"
#include "SDLWindow.h"

namespace DYE
{
    std::unique_ptr<ContextBase> ContextBase::Create(WindowBase *pWindow)
    {
        // TODO: add other context types. For instance, GLFWContext
        return nullptr;
    }
}