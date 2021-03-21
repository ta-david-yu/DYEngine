#include "Scene/ComponentBase.h"
#include "Scene/Entity.h"
#include "Util/Type.h"

#include <algorithm>
#include <SDL_log.h>

namespace DYE
{
    Entity * ComponentBase::GetEntityPtr() const
    {
        if (m_Entity.expired())
            return nullptr;
        else
            return m_Entity.lock().get();
    }

    std::string ComponentBase::GetComponentName() const
    {
        return std::move(getTypeName(*this));
    }

    ComponentUpdaterBase::ComponentUpdaterBase(ComponentTypeID typeID) : m_TypeID(typeID)
    {
    }

    std::weak_ptr<ComponentBase> ComponentUpdaterBase::AttachEntityWithComponent(std::weak_ptr<Entity> entity,
                                                         ComponentBase* component)
    {
        auto addedCompTypeID = std::type_index(typeid(*component));

        if (addedCompTypeID != GetTypeID())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "The attached component's type id {%s} doesn't match up with the updater's type id {%s}", addedCompTypeID.name(), GetTypeID().name());
        }

        std::shared_ptr<ComponentBase> sharedCompPtr(component);
        std::weak_ptr<ComponentBase> weakCompPtr = sharedCompPtr;

        /// attach component to the entity, assign entity reference to component
        entity.lock()->addComponent(addedCompTypeID, sharedCompPtr);
        component->m_Entity = entity;

        /// Add the entity/component to the list
        auto entID = entity.lock()->GetID();
        m_Components.emplace_back(entID, sharedCompPtr);

        /// call subclass implementation, for instance add the component to a list
        attachEntityWithComponent(std::move(entity), std::move(sharedCompPtr));

        return weakCompPtr;
    }

    bool ComponentUpdaterBase::EntityHasComponent(uint32_t entityID)
    {
        return std::any_of(m_Components.cbegin(), m_Components.cend(),
                           [entityID](const ComponentPair& pair)
                           {
                               return pair.first == entityID;
                           });
    }

    ComponentBase *ComponentUpdaterBase::GetComponentOfEntity(uint32_t entityID)
    {
        for (auto & pair : m_Components)
        {
            if (pair.first == entityID)
            {
                return pair.second.get();
            }
        }
        return nullptr;
    }

    //

    GenericComponentUpdater::GenericComponentUpdater(ComponentTypeID typeID) : ComponentUpdaterBase(typeID)
    {
    }

    void GenericComponentUpdater::UpdateComponents()
    {
        for (auto & pair : m_Components)
        {
            if (pair.second->GetIsEnabled())
                pair.second->OnUpdate();
        }
    }

    void GenericComponentUpdater::FixedUpdateComponents()
    {
        for (auto & pair : m_Components)
        {
            if (pair.second->GetIsEnabled())
                pair.second->OnFixedUpdate();
        }
    }

    void GenericComponentUpdater::RemoveComponentsOfEntity(uint32_t entityID)
    {
        m_Components.erase(
                std::remove_if(
                        m_Components.begin(),
                        m_Components.end(),
                        [entityID](const auto &compPair)
                        {
                            return compPair.first == entityID;
                        }), m_Components.end());
        /// TODO: better erase below
        /*
        for (auto it = m_Components.begin(); it != m_Components.end(); it++) {
            // remove components that has the given entityID
            if (it->first == entityID)
            {
                // to erase() but before erase() is executed
                m_Components.erase(it--);
            }
        }
         */
    }
}