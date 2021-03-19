#pragma once

#include "Scene/ComponentBase.h"
#include "Time.h"

namespace DYE
{
    class FrameCounterComponent : public ComponentBase
    {
    public:
        FrameCounterComponent() = default;

        void OnUpdate() override;

        double FpsAccumulator = 0;
        int FrameCounter = 0;
    };

    class FixedFrameCounterComponent : public ComponentBase
    {
    public:
        FixedFrameCounterComponent() = default;

        void OnUpdate() override { }
        void OnFixedUpdate() override;

        int FixedFrameCounter = 0;
    };
}