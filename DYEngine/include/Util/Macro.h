#pragma once

#include "Logger.h"

#include <utility>
#include <cstdio>
#include <cassert>
#include <cstdlib>

#ifdef DYE_DEBUG
    #define DYE_ENABLE_ASSERTS
#else
#endif

/// A helper macro that binds a member function into a lambda function; Hazel Engine: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Base.h
#define DYE_BIND_EVENT_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef DYE_ENABLE_ASSERTS
    /// \param condition Condition expression to check
    #define DYE_ASSERT(condition) SDL_assert((condition));
	#define DYE_ASSERT_LOG_WARN(condition, logFmt, ...) 		\
	do                                             		\
	{                                              		\
		if (!(condition))                           	\
		{                                               \
            char assertMessageBuffer[2048]; 			\
			int const result = sprintf(assertMessageBuffer, logFmt, ##__VA_ARGS__);	\
			DYE_LOG_WARN("Assertion Failed: %s", assertMessageBuffer);				\
			SDL_assert((condition)); 					\
		}												\
	} while (false)
#else
    #define DYE_ASSERT(condition)
	#define DYE_ASSERT_LOG_WARN(condition, logFmt, ...)
#endif

#ifndef NAME_OF
	#define NAME_OF(x) #x
#endif

#define DYE_ASSERT_RELEASE(condition) SDL_assert_release((condition))

#define DYE_MALLOC(size) std::malloc(size)
#define DYE_CALLOC(num, size) std::calloc(num, size)
#define DYE_FREE(ptr) std::free(ptr)