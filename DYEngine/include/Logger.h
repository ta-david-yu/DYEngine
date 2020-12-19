#pragma once

#include <SDL.h>

namespace DYE
{
    class Logger
    {
    public:
        static void Log(const char *fmt, ...);
    };
}

#define DYE_LOG(...) SDL_Log(__VA_ARGS__)

/// \param msgFlag should be one of the following: SDL_MESSAGEBOX_ERROR, SDL_MESSAGEBOX_WARNING, SDL_MESSAGEBOX_INFORMATION
/// \param title Title of the message window
/// \param msg Message of in the message box
#define DYE_MSG_BOX(msgFlag, title, msg) SDL_ShowSimpleMessageBox(msgFlag, title, msg, nullptr)