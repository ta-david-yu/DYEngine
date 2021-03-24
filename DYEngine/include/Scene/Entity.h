#pragma once

#include <typeindex>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <tuple>

namespace DYE
{
    class SceneLayer;
    class ComponentBase;
    class ComponentUpdaterBase;
    class Transform;

    /// An entity holds an ID that corresponds to a collection of components with logic
    class Entity
    {
        friend SceneLayer;
        friend ComponentUpdaterBase;
    public:
        /// Called by a factory when creating new ent
        /// \param id An unique runtime id, used to map components, constant across life time
        /// \param name The name of the entity
        Entity(int id, std::string name);

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        uint32_t GetID() const { return m_ID; }

        /// Get Component of the given typeID
        /// \param compTypeID
        /// \return true if the entity has the given component, otherwise false.
        /// In the case when the first return value is true, the second return value is the first component of the given type
        std::tuple<bool, std::weak_ptr<ComponentBase>> GetComponent(std::type_index compTypeID);

        /// Get Component or its subclass
        /// \tparam T The type of component
        /// \return true if the entity has the given component (or subclass), otherwise false.
        /// In the case when the first return value is true, the second return value is the first component of the given type (or subclass)
        template<typename T>
        std::tuple<bool, std::weak_ptr<T>> GetComponent()
        {
            for (const auto & pair : m_Components)
            {
                auto comp = pair.second.lock();
                auto castedComp = std::dynamic_pointer_cast<T>(comp);
                if (castedComp)
                {
                    return {true, castedComp};
                }
            }
            return {false, std::weak_ptr<T>{}};
        }

        std::weak_ptr<Transform> GetTransform() const { return m_Transform; }

    private:
        uint32_t m_ID;
        std::string m_Name;
        //std::multimap<std::type_index, std::weak_ptr<ComponentBase>> m_Components;
        std::vector<std::pair<std::type_index, std::weak_ptr<ComponentBase>>> m_Components;

        /// A cache to the transform component
        std::weak_ptr<Transform> m_Transform;

        /// Add a component to the map
        /// \param compTypeID
        /// \param component
        void addComponent(std::type_index compTypeID, const std::weak_ptr<ComponentBase>& component);
    };
}