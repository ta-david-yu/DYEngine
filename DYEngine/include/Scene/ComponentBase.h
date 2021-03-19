#pragma once

#include <map>
#include <vector>

namespace DYE
{
    /// The base interface for component, owned and updated by component updater
    class ComponentBase
    {
    public:
        virtual void OnUpdate() = 0;
        virtual void OnFixedUpdate() {}

        bool IsEnabled = true;
    };

    /// The abstract base interface for any component updater that is responsible for updating a list of components. It shouldn't be inherited directly.
    class ComponentUpdaterBase
    {
    public:
        ///
        /// \param order the update order of this updater in a scene layer
        explicit ComponentUpdaterBase(uint32_t order);
        virtual ~ComponentUpdaterBase() = default;

        virtual void UpdateComponents() = 0;
        virtual void FixedUpdateComponents() = 0;

        /// Check if an entity has the component that belongs to this updater
        /// \param entityId
        /// \return return true if the given entity id has the component, otherwise false
        virtual bool HasComponent(uint32_t entityID) = 0;

        /// Get a raw pointer to the component attached to the given entity id
        /// \param entityID
        /// \return the pointer to the component if the entity has the component, otherwise null ptr
        virtual ComponentBase* GetComponentWithEntityID(uint32_t entityID) = 0;

        /// Get the update order in the SceneLayer, the smaller the value, the earlier the component updater get updated in a loop. It's always unique
        int GetUpdateOrder() const { return m_UpdateOrder; }
    protected:
        /// Update Order in the SceneLayer, the smaller the value, the earlier the component updater get updated in a loop. It's always unique
        uint32_t m_UpdateOrder = 0;
    };

    /// The template base interface for any component updater
    /// \tparam T the type of the responsible component, should always inherited from ComponentBase
    template<typename T>
    class ComponentUpdater : ComponentUpdaterBase
    {
        /// \param uint32_t entityID
        /// \param shared_ptr  to the component
        using ComponentPair = std::pair<uint32_t, std::shared_ptr<T>>;
    public:
        explicit ComponentUpdater(uint32_t order);

        void UpdateComponents() override;
        void FixedUpdateComponents() override;

        bool HasComponent(uint32_t entityID) override;
        ComponentBase * GetComponentWithEntityID(uint32_t entityID) override;
    protected:
        std::vector<ComponentPair> m_Components;
    };

    template<typename T>
    ComponentUpdater<T>::ComponentUpdater(uint32_t order) : ComponentUpdaterBase(order)
    {}

    template<typename T>
    void ComponentUpdater<T>::UpdateComponents()
    {
        for (int i = 0; m_Components.size(); i++)
        {
            ComponentPair& pair = m_Components[i];
            pair.second.OnUpdate();
        }
    }

    template<typename T>
    void ComponentUpdater<T>::FixedUpdateComponents()
    {
        for (int i = 0; m_Components.size(); i++)
        {
            ComponentPair& pair = m_Components[i];
            pair.second.OnFixedUpate();
        }
    }

    template<typename T>
    bool ComponentUpdater<T>::HasComponent(uint32_t entityID)
    {
        for (int i = 0; m_Components.size(); i++)
        {
            ComponentPair& pair = m_Components[i];
            if (pair.first == entityID)
            {
                return true;
            }
        }
        return false;
    }

    template<typename T>
    ComponentBase *ComponentUpdater<T>::GetComponentWithEntityID(uint32_t entityID)
    {
        for (int i = 0; m_Components.size(); i++)
        {
            ComponentPair& pair = m_Components[i];
            if (pair.first == entityID)
            {
                return pair.second.get();
            }
        }
        return nullptr;
    }
}