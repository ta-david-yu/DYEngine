#include "Core/LayerBase.h"

#include <utility>

namespace DYE
{
    LayerBase::LayerBase(std::string name) : m_Name(std::move(name))
    {

    }
}
