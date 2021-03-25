#include "FrameCounterComponent.h"

#include "Time.h"

#if DYE_DEBUG
#include <imgui.h>
#endif

namespace DYE
{
    void FrameCounterComponent::OnUpdate()
    {
        // FPS
        FrameCounter++;
        FpsAccumulator += TIME.DeltaTime();
        if (FpsAccumulator >= 0.25)
        {
            FPS = FrameCounter / FpsAccumulator;
            //SDL_Log("Delta FPS: %f", fps);

            FrameCounter = 0;
            FpsAccumulator = 0;
        }
    }

#if DYE_DEBUG
    void FrameCounterComponent::onComponentDebugWindowGUI(float width, float height)
    {
        ComponentBase::onComponentDebugWindowGUI(width, height);

        ImGui::Text("FPS: %f", FPS);
        ImGui::Text("FrameCounter: %d", FrameCounter);
        ImGui::Text("FpsAccumulator: %f", FpsAccumulator);
    }
#endif

    void FixedFrameCounterComponent::OnFixedUpdate()
    {
        FixedFrameCounter++;
    }
}