#pragma once

#include <string>

namespace DYE
{
    /// An entity holds an ID that corresponds to a collection of components with logic
    class Entity
    {
        friend class SceneLayer;
    public:
        /// Called by a factory when creating new ent
        /// \param id An unique runtime id, used to map components, constant across life time
        /// \param name The name of the entity
        Entity(int id, std::string name);

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        int GetID() const { return m_ID; }

    private:
        uint32_t m_ID;
        std::string m_Name;
    };
}