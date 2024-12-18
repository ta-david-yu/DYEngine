#pragma once

#include <string>
#include <typeinfo>
#include <concepts>

namespace DYE::TypeUtil
{
    std::string demangleCTypeName(const char *name);

    /// Get the demangled typename of the given object's type
    /// \tparam T
    /// \param t
    /// \return a string of the name
    template<class T>
    std::string GetTypeName(const T &t)
    {
        return demangleCTypeName(typeid(t).name());
    }

    template<class T, class U>
    concept DerivedFrom = std::is_base_of<U, T>::value;
}