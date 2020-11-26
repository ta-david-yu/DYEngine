#pragma once

#include <utility>

/// A helper macro that binds a member function into a lambda function
/// ;Hazel Engine: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Base.h
#define DYE_BIND_EVENT_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }