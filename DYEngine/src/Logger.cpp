#include "Util/Logger.h"

#include <cstdio>
#include <iostream>
#include <cassert>
#include <cstdarg>

#define LOG_BUFFER_SIZE 2048

namespace DYE
{
    /// Declare it here so Print func can use it
    void customPrint(LogType type, const char *file, int line, const char *msg, va_list args);

    void Print(LogType type, const char *file, int line, const char *msg, ...)
    {
        va_list args;
        va_start(args, msg);
        customPrint(type, file, line, msg, args);
        va_end(args);
    }

    void customPrint(LogType type, const char *file, int line, const char *msg, va_list args)
    {
        char formattedMsg[LOG_BUFFER_SIZE];
        char positionMsg[LOG_BUFFER_SIZE];

#if defined(_MSC_VER)
        int offset = vsprintf_s(formattedMsg, msg, args);
#else
        int offset = std::vsprintf(formattedMsg, msg, args);
#endif
        int length = std::snprintf(formattedMsg + offset, LOG_BUFFER_SIZE, "\n\tat %-30s:%d", file, line);

        // print
        switch (type)
        {
            case Info:
                SDL_Log("%s", formattedMsg);
                break;
            case Debug:
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", formattedMsg);
                break;
            case Error:
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", formattedMsg);
                break;
            case Warning:
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", formattedMsg);
                break;


            case NumOfType:
                break;
        }
    }
}