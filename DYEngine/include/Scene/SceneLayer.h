#pragma once

#include "LayerBase.h"
#include "Scene/ComponentBase.h"

#include <vector>
#include <typeinfo>
#include <map>
#include <tuple>

namespace DYE
{
    class Entity;
    class ComponentUpdaterBase;
    class WindowBase;

    /// A Layer that holds a scene + entity-component system
    class SceneLayer : public LayerBase
    {
    public:
        explicit SceneLayer(WindowBase* pWindow);
        SceneLayer() = delete;
        ~SceneLayer();

        void OnEvent(const std::shared_ptr<Event> &pEvent) override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
        void OnImGui() override;

        /// Create an entity and push into the list
        /// \param name the name of the entity
        /// \return a weak pointer to the entity
        std::weak_ptr<Entity> CreateEntity(const std::string& name);

        /// Delayed destroy an entity with the given id at the end of this frame
        /// \param entityID
        void DestroyEntity(uint32_t entityID);

        /// Get the entity with the given id
        /// \param id
        /// \return a raw pointer to the entity. Return nullptr if the entity with the given id doesn't exist
        Entity* GetEntity(int id);

        /// Create a generic updater that is responsible for updating the components of the given component type id
        /// \param typeID type_index of the component type
        /// \return a weak pointer to the newly created and registered component updater
        std::weak_ptr<ComponentUpdaterBase> CreateAndRegisterGenericComponentUpdater(ComponentTypeID typeID);

        /// Register an updater that created by the user manually, useful for specialized updater, such as RendererUpdater or PhysicsUpdater
        /// \param typeID
        /// \param updater the updater to be registered
        void RegisterComponentUpdater(const std::shared_ptr<ComponentUpdaterBase>& updater);

        /// Get the component updater that is responsible for updating the components of the given type. Used during adding a new component to an entity
        /// \param typeID type_index of the component type
        /// \return a raw pointer to the updater. If the updater of the given type is not registered, return nullptr
        ComponentUpdaterBase* GetComponentUpdaterOfType(ComponentTypeID typeID);

    private:
        WindowBase* m_pWindow;

        /// The latest created entity's id
        uint32_t m_EntityIDCounter = 0;
        std::vector<std::shared_ptr<Entity>> m_Entities;

        /// A list of pointers to registered component updaters, it's raw because the life time of them is the same as the scene layer
        std::vector<std::shared_ptr<ComponentUpdaterBase>> m_ComponentUpdaters;

        /// A map that maps the TypeID to the index of that updater in m_ComponentUpdaters
        std::map<ComponentTypeID, uint32_t> m_ComponentTypeIDToComponentUpdaterIndex;

        ///
        /// \param typeID
        /// \return true if the given typeID is in the vector, otherwise false. In the case when the first return value is true, the second return value is the index of the updater in the vector
        std::tuple<bool, uint32_t> getUpdaterIndex(ComponentTypeID typeID) const
        {
            auto pair = m_ComponentTypeIDToComponentUpdaterIndex.find(typeID);
            if (pair == m_ComponentTypeIDToComponentUpdaterIndex.end())
            {
                return {false, 0};
            }
            else
            {
                return {true, pair->second};
            }
        }
    };
}
