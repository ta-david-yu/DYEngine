#include <utility>

#include "Scene/Entity.h"


namespace DYE
{
    Entity::Entity(int id, std::string name) : m_ID(id), m_Name(std::move(name))
    {
    }
}
