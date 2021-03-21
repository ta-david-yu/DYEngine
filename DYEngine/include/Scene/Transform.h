#pragma once

#include "Scene/ComponentBase.h"

namespace DYE
{
    class Transform : public ComponentBase
    {
    public:
        Transform() = default;

        void OnUpdate() override;
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