#pragma once

#include <map>
#include <vector>
#include <typeindex>
#include <memory>

namespace DYE
{
    using ComponentTypeID = std::type_index;

    class SceneLayer;
    class Entity;
    class ComponentUpdaterBase;

    /// The base interface for component, owned and updated by component updater
    class ComponentBase
    {
        friend SceneLayer;
        friend ComponentUpdaterBase;
    public:
        ComponentBase() = default;
        virtual ~ComponentBase() = default;

        virtual void OnUpdate() = 0;
        virtual void OnFixedUpdate() {}

        bool GetIsEnabled() const { return m_IsEnabled; }
        void SetIsEnabled(bool enabled) { m_IsEnabled = enabled; }

        /// Get entity this component belongs to
        /// \return a raw pointer to the entity if the entity exists, otherwise nullptr
        Entity* GetEntityPtr() const;

    private:
        bool m_IsEnabled = true;
        std::weak_ptr<Entity> m_Entity;
    };

    /// The abstract base interface for any component updater that is responsible for updating a list of components. It shouldn't be inherited directly.
    class ComponentUpdaterBase
    {
        friend SceneLayer;
    public:
        /// \param uint32_t entityID
        /// \param shared_ptr  to the component
        using ComponentPair = std::pair<uint32_t, std::shared_ptr<ComponentBase>>;

        ///
        /// \param order the update order of this updater in a scene layer
        explicit ComponentUpdaterBase(ComponentTypeID typeID);
        virtual ~ComponentUpdaterBase() = default;

        virtual void UpdateComponents() = 0;
        virtual void FixedUpdateComponents() = 0;

        /// Attach the given component to the entity, and register this component to the updater list
        /// \param entity the entity to be attached to
        /// \param component the component to be attached and registered
        /// \return a weak pointer to the component
        std::weak_ptr<ComponentBase> AttachEntityWithComponent(std::weak_ptr<Entity> entity, ComponentBase* component);

        /// Check if an entity has the component that belongs to this updater
        /// \param entityId
        /// \return return true if the given entity id has the component, otherwise false
        bool EntityHasComponent(uint32_t entityID);

        /// Get the first raw pointer to the component attached to the given entity id in the list
        /// \param entityID
        /// \return the raw pointer to the component if the entity has the component, otherwise null ptr
        ComponentBase* GetComponentOfEntity(uint32_t entityID);

        /// Remove all the components that are attached to the entity with the given entityID
        /// \param entityID 
        virtual void RemoveComponentsOfEntity(uint32_t entityID) = 0;
        
        
        /// Get the unique type identifier of the component managed by this updater
        /// \return
        ComponentTypeID GetTypeID() const { return m_TypeID; }

    protected:
        /// An unique type identifier of the component managed by this updater
        ComponentTypeID m_TypeID;

        std::vector<ComponentPair> m_Components;

        virtual void attachEntityWithComponent(const std::weak_ptr<Entity>& entity, const std::shared_ptr<ComponentBase>& component) { };
    };

    /// The single generic component updater, responsible for one generic custom component
    class GenericComponentUpdater final : public ComponentUpdaterBase
    {
    public:
        explicit GenericComponentUpdater(ComponentTypeID typeID);

        void UpdateComponents() override;
        void FixedUpdateComponents() override;

        void RemoveComponentsOfEntity(uint32_t entityID) override;
    };
}