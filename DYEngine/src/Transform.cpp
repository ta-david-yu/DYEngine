#include "Scene/Transform.h"

#include <algorithm>

#if DYE_DEBUG
#include <imgui.h>
#endif

namespace DYE
{
    void Transform::OnUpdate()
    {
    }

#if DYE_DEBUG
    void Transform::onComponentDebugWindowGUI(float width, float height)
    {
        ComponentBase::onComponentDebugWindowGUI(width, height);
        ImGui::Text("Position"); ImGui::SameLine(); ImGui::DragFloat3("", (float*)&m_Position, 1, 0, 0);
    }
#endif

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