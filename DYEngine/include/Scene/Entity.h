#pragma once

#include <typeindex>
#include <map>
#include <string>

namespace DYE
{
    class SceneLayer;
    class ComponentBase;
    class ComponentUpdaterBase;

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

    private:
        uint32_t m_ID;
        std::string m_Name;
        std::multimap<std::type_index, std::weak_ptr<ComponentBase>> m_Components;

        /// Add a component to the map
        /// \param compTypeID
        /// \param component
        void addComponent(std::type_index compTypeID, std::weak_ptr<ComponentBase> component);
    };
}