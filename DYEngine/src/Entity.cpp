#include <utility>

#include "Scene/Entity.h"


namespace DYE
{
    Entity::Entity(int id, std::string name) : m_ID(id), m_Name(std::move(name))
    {
    }

    std::tuple<bool, std::weak_ptr<ComponentBase>> Entity::GetComponent(std::type_index compTypeID)
    {
        for (auto & pair : m_Components)
        {
            if (pair.first == compTypeID)
            {
                return {true, pair.second};
            }
        }
        return {false, std::weak_ptr<ComponentBase>{}};
    }

    void Entity::addComponent(std::type_index compTypeID, const std::weak_ptr<ComponentBase>& component)
    {
        m_Components.emplace_back(compTypeID, component);
    }
}
