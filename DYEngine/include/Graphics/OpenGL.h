#pragma once

#include "Util/Logger.h"

#include <glad/glad.h>
#include <string>

namespace DYE
{
    std::string glGetErrorMessage();
    bool glCheckError(const char* functionName);
}

#ifdef DYE_DEBUG

// glGetError to clear previous buffered error
#define glCall(glFunction)						                                        \
	do											                                        \
	{                                                                                   \
		glGetError();							                                        \
		glFunction;								                                        \
		if (glCheckError(#glFunction))			                                        \
			DYE_LOG_WARN(glGetErrorMessage().c_str());    \
	} while (false)

#define glCheckAfterCall(glFunction)			                                        \
	do											                                        \
	{                                                                                   \
		if (glCheckError(#glFunction))			                                        \
			DYE_LOG_WARN(glGetErrorMessage().c_str());	\
	} while (false)

#else

#define glCall(glFunction) glFunction
#define glCheckAfterCall(glFunction) glFunction

#endif // DEBUG