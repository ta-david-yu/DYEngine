#pragma once

#include "LayerBase.h"

#include <vector>
#include <map>


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

        /// Get the entity with the given id
        /// \param id
        /// \return a raw pointer to the entity. Return nullptr if the entity with the given id doesn't exist
        Entity* GetEntity(int id);

        /// Create a updater that is responsible for updating the components of the given type
        /// \tparam T type of the component that this updater is responsible for
        /// \return a raw pointer to the newly created and registered component updater
        template<class T>
        ComponentUpdaterBase* CreateAndRegisterComponentUpdater();

        /// Get the component updater that is responsible for updating the components of the given type. Used during adding a new component to an entity
        /// \tparam T type of the component that this updater is responsible for
        /// \return a raw pointer to the updater. If the updater of the given type is not registered, return nullptr
        template<class T>
        ComponentUpdaterBase* GetComponentUpdaterOfType();

    private:
        WindowBase* m_pWindow;

        /// The latest created entity's id
        uint32_t m_EntityIDCounter = 0;
        std::vector<std::shared_ptr<Entity>> m_Entities;

        /// A list of pointers to registered component updaters, it's raw beacuase the life time of them is the same as the scene layer
        std::vector<ComponentUpdaterBase*> m_pComponentUpdaters;

    };
}
