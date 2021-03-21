#pragma once

#include "Scene/ComponentBase.h"

#include <glm/glm.hpp>

namespace DYE
{
    class Transform : public ComponentBase
    {
    public:
        Transform() = default;

        void OnUpdate() override;

        glm::vec3& GetPosition() { return m_Position; }
    private:
        glm::vec3 m_Position {0, 0, 0};
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