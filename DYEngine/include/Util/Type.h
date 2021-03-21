#pragma once

#include <string>
#include <typeinfo>

std::string demangleCTypeName(const char* name);

/// Get the demangled typename of the given object's type
/// \tparam T
/// \param t
/// \return a string of the name
template <class T>
std::string getTypeName(const T& t) {

    return demangleCTypeName(typeid(t).name());
}