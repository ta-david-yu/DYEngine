#pragma once

#include "SDL.h"

namespace DYE
{
    enum LogType
    {
        Info,
        Debug,
        Warning,
        Error,

        NumOfType
    };

    /// Log with LogType
    /// \param type
    /// \param file filename
    /// \param line line of the filename
    /// \param msg formatting message
    /// \param ...
    void Print(LogType type, const char* file, int line, const char* msg, ...);
}

/// Log without line, file information
/// \param fmt formatting string
#define DYE_LOG(fmt, ...) SDL_Log(fmt, ##__VA_ARGS__)

/// \param fmt formatting string
#define DYE_LOG_INFO(fmt, ...) Print(LogType::Info, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/// \param fmt formatting string
#define DYE_LOG_DEBUG(fmt, ...) Print(LogType::Debug, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/// \param fmt formatting string
#define DYE_LOG_WARN(fmt, ...) Print(LogType::Warning, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/// \param fmt formatting string
#define DYE_LOG_ERROR(fmt, ...) Print(LogType::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)


/// \param msgFlag should be one of the following: SDL_MESSAGEBOX_ERROR, SDL_MESSAGEBOX_WARNING, SDL_MESSAGEBOX_INFORMATION
/// \param title Title of the message window
/// \param msg Message of in the message box
#define DYE_MSG_BOX(msgFlag, title, msg) SDL_ShowSimpleMessageBox(msgFlag, title, msg, nullptr)