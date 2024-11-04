#include "Input/KeyCode.h"

namespace DYE
{
    std::string GetKeyName(KeyCode keyCode)
    {
        return SDL_GetKeyName(static_cast<SDL_KeyCode>(keyCode));
    }
}