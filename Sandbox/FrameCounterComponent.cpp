#include "FrameCounterComponent.h"

#include "Time.h"

namespace DYE
{
    void FrameCounterComponent::OnUpdate()
    {
        // FPS
        FrameCounter++;
        FpsAccumulator += TIME.DeltaTime();
        if (FpsAccumulator >= 0.25)
        {
            double fps = FrameCounter / FpsAccumulator;
            //SDL_Log("Delta FPS: %f", fps);

            FrameCounter = 0;
            FpsAccumulator = 0;
        }
    }

    void FixedFrameCounterComponent::OnFixedUpdate()
    {
        FixedFrameCounter++;
    }
}