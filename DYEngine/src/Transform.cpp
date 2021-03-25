#include "Scene/Transform.h"
#include "Scene/Entity.h"

#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <utility>

#if DYE_DEBUG
#include <imgui.h>
#endif

namespace DYE
{
    Transform::~Transform()
    {
        /// Old parent exists, de-register from its children list
        if (!m_Parent.expired())
        {
            auto ent = GetEntityPtr();
            auto entityID = ent->GetID();

            auto oldParent = m_Parent.lock();
            oldParent->m_Children.erase(
                    std::remove_if(
                            oldParent->m_Children.begin(),
                            oldParent->m_Children.end(),
                            [entityID](const auto &childTransform)
                            {
                                return childTransform->GetEntityPtr()->GetID() == entityID;
                            }), oldParent->m_Children.end());
        }
    }

    void Transform::OnUpdate()
    {
    }

    void Transform::SetParent(std::weak_ptr<Transform> parent)
    {
        /// Old parent exists, de-register from its children list
        if (!m_Parent.expired())
        {
            auto ent = GetEntityPtr();
            auto entityID = ent->GetID();

            auto oldParent = m_Parent.lock();
            oldParent->m_Children.erase(
                    std::remove_if(
                            oldParent->m_Children.begin(),
                            oldParent->m_Children.end(),
                            [entityID](const auto &childTransform)
                            {
                                return childTransform->GetEntityPtr()->GetID() == entityID;
                            }), oldParent->m_Children.end());
        }

        m_Parent = std::move(parent);

        /// If new parent exists, register to its children list
        if (!m_Parent.expired())
        {
            m_Parent.lock()->m_Children.push_back(this);
        }
    }

#if DYE_DEBUG
    void Transform::onComponentDebugWindowGUI(float width, float height)
    {
        ComponentBase::onComponentDebugWindowGUI(width, height);

        ImGui::PushItemWidth(width * 0.5f / 3);

        ImGui::Text("Parent: ");
        ImGui::SameLine();
        if (!GetParent().expired())
        {
            auto parentEntity = GetParent().lock()->GetEntityPtr();

            char parentLabel[256];
            sprintf(parentLabel, "[ID: %d] %s##", parentEntity->GetID(), parentEntity->GetName().c_str());
            ImGui::Text("%s", parentLabel);
        }
        else
        {
            ImGui::Text("null");
        }

        if (!m_Children.empty())
        {
            //ImGui::Text("Children - %zu", m_Children.size());
            //ImGui::SameLine();
            char childNodeLabel[256];
            sprintf(childNodeLabel, "Children - %zu", m_Children.size());
            bool openNode = ImGui::TreeNode(childNodeLabel);
            if (openNode)
            {
                for (const auto& child : m_Children)
                {
                    auto childEntity = child->GetEntityPtr();

                    char childLabel[256];
                    sprintf(childLabel, "[ID: %d] %s", childEntity->GetID(), childEntity->GetName().c_str());
                    ImGui::Text("%s", childLabel);
                }

                ImGui::TreePop();
            }
        }
        else
        {
            ImGui::Text("No child transform");
        }

        ImGui::Text("Position");
        ImGui::SameLine(); ImGui::DragFloat("x##position", (float*)&m_Position.x, 1, 0, 0);
        ImGui::SameLine(); ImGui::DragFloat("y##position", (float*)&m_Position.y, 1, 0, 0);
        ImGui::SameLine(); ImGui::DragFloat("z##position", (float*)&m_Position.z, 1, 0, 0);

/*
        auto euler = glm::degrees(glm::eulerAngles(m_Rotation));
        ImGui::Text("%f %f %f", euler.x, euler.y, euler.z);
        ImGui::Text("Rotation");
        ImGui::SameLine(); ImGui::DragFloat("x##rotation", (float*)&euler.x, 1, 0, 0);
        ImGui::SameLine(); ImGui::DragFloat("y##rotation", (float*)&euler.y, 1, 0, 0);
        ImGui::SameLine(); ImGui::DragFloat("z##rotation", (float*)&euler.z, 1, 0, 0);
        auto newQuat = glm::quat { glm::orientate3(glm::radians(glm::vec3{euler.y, euler.x, euler.z})) };
        m_Rotation = glm::normalize(newQuat);
        */
        //ImGui::PopItemWidth();


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
        /// TODO: better erasing function below
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