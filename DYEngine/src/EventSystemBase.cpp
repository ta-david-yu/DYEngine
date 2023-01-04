#include "Event/EventSystemBase.h"
#include "Event/SDLEventSystem.h"
#include "Util/Macro.h"

namespace DYE
{
    std::unique_ptr<EventSystemBase> EventSystemBase::Create()
    {
        // TODO: add other event system types. For instance, GLFWEventSystem
        return std::make_unique<SDLEventSystem>();
    }
}