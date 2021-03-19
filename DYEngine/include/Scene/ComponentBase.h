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
    public:
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
        virtual std::weak_ptr<ComponentBase> AttachEntityWithComponent(std::weak_ptr<Entity> entity, ComponentBase* component);

        /// Check if an entity has the component that belongs to this updater
        /// \param entityId
        /// \return return true if the given entity id has the component, otherwise false
        virtual bool HasComponent(uint32_t entityID) = 0;

        /// Get the first raw pointer to the component attached to the given entity id in the list
        /// \param entityID
        /// \return the raw pointer to the component if the entity has the component, otherwise null ptr
        virtual ComponentBase* GetComponentWithEntityID(uint32_t entityID) = 0;


        /// Get the unique type identifier of the component managed by this updater
        /// \return
        ComponentTypeID GetTypeID() const { return m_TypeID; }
    protected:
        /// An unique type identifier of the component managed by this updater
        ComponentTypeID m_TypeID;
        /// Update Order in the SceneLayer, the smaller the value, the earlier the component updater get updated in a loop. It's always unique
        uint32_t m_UpdateOrder = 0;

        virtual void attachEntityWithComponent(std::weak_ptr<Entity> entity, std::shared_ptr<ComponentBase> component) = 0;
    };

    /// The single generic component updater, responsible for one generic custom component
    class GenericComponentUpdater final : public ComponentUpdaterBase
    {
        /// \param uint32_t entityID
        /// \param shared_ptr  to the component
        using ComponentPair = std::pair<uint32_t, std::shared_ptr<ComponentBase>>;
    public:
        explicit GenericComponentUpdater(ComponentTypeID typeID);

        void UpdateComponents() override;
        void FixedUpdateComponents() override;

        bool HasComponent(uint32_t entityID) override;
        ComponentBase * GetComponentWithEntityID(uint32_t entityID) override;

    private:
        std::vector<ComponentPair> m_Components;
        void attachEntityWithComponent(std::weak_ptr<Entity> entity, std::shared_ptr<ComponentBase> component) override;
    };
}