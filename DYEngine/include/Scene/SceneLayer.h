#pragma once

#include "LayerBase.h"
#include "Scene/ComponentBase.h"
#include "Event/KeyEvent.h"

#include <vector>
#include <typeinfo>
#include <map>
#include <tuple>

namespace DYE
{
    class Entity;
    class ComponentUpdaterBase;
    class TransformUpdater;
    class ImageRendererUpdater;
    class WindowBase;

    /// A Layer that holds a scene + entity-component system
    class SceneLayer : public LayerBase
    {
    public:
        explicit SceneLayer(WindowBase* pWindow);
        SceneLayer() = delete;
        ~SceneLayer() override;

        /// Setup some of the default and built-in updaters, such as TransformUpdater, RendererUpdater
        void SetupDefaultUpdaters();

        void OnInit() override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
        void OnRender() override;
        void OnImGui() override;
        void OnEvent(Event& event) override;

        /// Create an entity and push into the list
        /// \param name the name of the entity
        /// \return a weak pointer to the entity
        std::weak_ptr<Entity> CreateEntity(const std::string& name);

        /// Delayed destroy an entity with the given id at the end of this frame
        /// \param entityID
        void ImmediateDestroyEntity(uint32_t entityID);

        /// Get the entity with the given id
        /// \param id
        /// \return a raw pointer to the entity. Return nullptr if the entity with the given id doesn't exist
        Entity* GetEntity(uint32_t id);

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

        /// Get the component updater that is responsible for updating the components of the given type. Used during adding a new component to an entity
        /// \tparam T the component type
        /// \return a raw pointer to the updater. If the updater of the given type is not registered, return nullptr
        template<typename T>
        ComponentUpdaterBase* GetComponentUpdaterOfType();

        /// Get the component updater that is responsible for updating the components of the given type. Used during adding a new component to an entity
        /// \tparam ComponentType the component type, use as a key to retrieve the updater
        /// \tparam UpdaterType the updater type, the return pointer is cast to the type* first
        /// \return a raw pointer to the updater. If the updater of the given type is not registered, return nullptr
        template<typename ComponentType, typename UpdaterType>
        UpdaterType* GetComponentUpdaterOfType();

        /// Create a component and attach it to the entity. If an updater with the given typeID exists, register the component to it. Otherwise, a new generic updater will be instantiated and registered
        /// \tparam T the type of the component
        /// \param entity the to-be-attached-to entity
        /// \param typeID the typeID that is used to find/initialize the updater
        /// \return a weak pointer to the newly created component
        template<typename T>
        std::weak_ptr<T> LazyAddComponentToEntity(std::weak_ptr<Entity> entity, ComponentTypeID typeID);

        /// Create a component and attach it to the entity. If an updater with the given typeid(componentType) exists, register the component to it. Otherwise, a new generic updater will be instantiated and registered
        /// \tparam T the type of the component
        /// \param entity the to-be-attached-to entity
        /// \return a weak pointer to the newly created component
        template<typename T>
        std::weak_ptr<T> LazyAddComponentToEntity(std::weak_ptr<Entity> entity);

    private:
        WindowBase* m_pWindow;

        /// The latest created entity's id
        uint32_t m_EntityIDCounter = 0;

        ///
        /// \param uint32_t unique entityID
        /// \param sharedPtr2Entity a shared_ptr to Entity
        std::map<uint32_t, std::shared_ptr<Entity>> m_Entities;
        //std::vector<std::shared_ptr<Entity>> m_Entities;

        // Important Updaters
        std::weak_ptr<TransformUpdater> m_TransformUpdater;
        std::weak_ptr<ImageRendererUpdater> m_ImageRendererUpdater;

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

#if DYE_DEBUG
        bool m_SceneDebugWindowIsOpen = true;

        bool m_ComponentDebugWindowIsOpen = false;
        std::weak_ptr<ComponentBase> m_DebugTargetComponent;

        bool m_UpdaterDebugWindowIsOpen = false;
        std::weak_ptr<ComponentUpdaterBase> m_DebugTargetUpdater;
#endif
    };

    template<typename T>
    ComponentUpdaterBase *SceneLayer::GetComponentUpdaterOfType()
    {
        // delete the registered component updater
        for (auto& updater : m_ComponentUpdaters)
        {
            if (updater->GetTypeID() == ComponentTypeID(typeid(T)))
            {
                return updater.get();
            }
        }
        return nullptr;
    }

    template<typename ComponentType, typename UpdaterType>
    UpdaterType *SceneLayer::GetComponentUpdaterOfType()
    {
        // delete the registered component updater
        for (auto& updater : m_ComponentUpdaters)
        {
            if (updater->GetTypeID() == ComponentTypeID(typeid(ComponentType)))
            {
                return dynamic_cast<UpdaterType*>(updater.get());
            }
        }
        return nullptr;
    }

    template<typename T>
    std::weak_ptr<T> SceneLayer::LazyAddComponentToEntity(std::weak_ptr<Entity> entity, ComponentTypeID typeID)
    {
        auto updater = GetComponentUpdaterOfType(typeID);

        // if it doesn't exist already, create a new one
        if (updater == nullptr)
        {
            updater = CreateAndRegisterGenericComponentUpdater(typeID).lock().get();
        }

        auto newComp = updater->AttachEntityWithComponent(entity, new T());
        return std::static_pointer_cast<T>(newComp.lock());
    }

    template<typename T>
    std::weak_ptr<T> SceneLayer::LazyAddComponentToEntity(std::weak_ptr<Entity> entity)
    {
        ComponentTypeID typeID = ComponentTypeID(typeid(T));

        auto updater = GetComponentUpdaterOfType(typeID);

        // if it doesn't exist already, create a new one
        if (updater == nullptr)
        {
            updater = CreateAndRegisterGenericComponentUpdater(typeID).lock().get();
        }

        auto newComp = updater->AttachEntityWithComponent(entity, new T());
        return std::static_pointer_cast<T>(newComp.lock());
    }
}
