#include "Util/TypeUtil.h"

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace DYE::TypeUtil
{
    std::string demangleCTypeName(const char *name)
    {
        int status = -4; // some arbitrary value to eliminate the compiler warning

        // enable c++11 by passing the flag -std=c++11 to g++
        std::unique_ptr<char, void (*)(void *)> res {
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
        };

        return (status == 0) ? res.get() : name;
    }
}

#else

namespace DYE::Type
{
    // does nothing if not g++
    std::string demangle(const char *name)
    {
        return name;
    }
}

#endif