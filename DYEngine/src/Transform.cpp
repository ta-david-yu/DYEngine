#include "Scene/Transform.h"

#include <algorithm>

namespace DYE
{
    void Transform::OnUpdate()
    {
    }

    TransformUpdater::TransformUpdater(ComponentTypeID typeID) : ComponentUpdaterBase(typeID)
    {
    }

    void TransformUpdater::UpdateComponents()
    {
        /// TODO: update local transform
    }

    void TransformUpdater::FixedUpdateComponents()
    {
        /// TODO: update local transform
    }

    void TransformUpdater::RemoveComponentsOfEntity(uint32_t entityID)
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