#include <utility>

#include "Scene/GameObject.h"


namespace DYE
{
    GameObject::GameObject(int id, std::string name) : m_ID(id), m_Name(std::move(name))
    {
    }

    std::tuple<bool, std::weak_ptr<ComponentBase>> GameObject::GetComponent(std::type_index compTypeID)
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

    void GameObject::addComponent(std::type_index compTypeID, const std::weak_ptr<ComponentBase>& component)
    {
        m_Components.emplace_back(compTypeID, component);
    }
}
