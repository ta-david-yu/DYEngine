#pragma once

#include "Scene/ComponentBase.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace DYE
{
    class SceneLayer;

    class Transform final : public ComponentBase
    {
        friend SceneLayer;
    public:
        Transform() = default;
        ~Transform() final;

        void OnUpdate() override;

        glm::vec3 GetLocalPosition() const { return m_Position; }
        glm::quat GetLocalRotation() const { return m_Rotation; }
        glm::vec3 GetLocalScale() const { return m_Scale; }

        void SetLocalPosition(glm::vec3 pos) { m_Position = pos; }
        void SetLocalScale(glm::vec3 scale) { m_Scale = scale; }

        std::weak_ptr<Transform> GetParent() const { return m_Parent; }
        void SetParent(std::weak_ptr<Transform> parent);

        glm::mat4 GetTransformMatrix() const
        {
            return glm::translate(m_Position) *
                   glm::toMat4(m_Rotation) *
                   glm::scale(m_Scale);
        }

    private:
        glm::vec3 m_Position {0, 0, 0};
        glm::quat m_Rotation {0, 0, 0, 0};
        glm::vec3 m_Scale = {1, 1, 1};

        std::weak_ptr<Transform> m_Parent;
        std::vector<Transform*> m_Children;

    protected:
#if DYE_DEBUG
        void onComponentDebugWindowGUI(float width, float height) override;
#endif
    };

    /// The updater responsible for updating transform
    class TransformUpdater final : public ComponentUpdaterBase
    {
    public:
        explicit TransformUpdater(ComponentTypeID typeID);

        void UpdateComponents() override;
        void FixedUpdateComponents() override;

        void RemoveComponentsOfEntity(uint32_t entityID) override;
    };
}