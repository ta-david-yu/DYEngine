#include <utility>

#include "Scene/Entity.h"


namespace DYE
{
    Entity::Entity(int id, std::string name) : m_ID(id), m_Name(std::move(name))
    {
    }

    void Entity::addComponent(std::type_index compTypeID, std::weak_ptr<ComponentBase> component)
    {
        m_Components.emplace(compTypeID, component);
    }
}
