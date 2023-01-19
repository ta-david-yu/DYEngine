#pragma once

#include "Logger.h"

#include <utility>
#include <cassert>

#ifdef DYE_DEBUG
    #define DYE_ENABLE_ASSERTS
#else
#endif

/// A helper macro that binds a member function into a lambda function; Hazel Engine: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Base.h
#define DYE_BIND_EVENT_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef DYE_ENABLE_ASSERTS
    /// \param condition Condition expression to check
    #define DYE_ASSERT(condition) SDL_assert((condition));
#else
    #define DYE_ASSERT(condition)
#endif

#ifndef NAME_OF
	#define NAME_OF(x) #x
#endif

#define DYE_ASSERT_RELEASE(condition) SDL_assert_release((condition))