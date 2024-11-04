#pragma once

#include "Graphics/Camera.h"

#include "Components/TransformComponents.h"

namespace DYE::DYEditor
{
    struct CameraComponent
    {
        bool IsEnabled = true;
        CameraProperties Properties;

        [[nodiscard]]
        Camera CreateCameraWithLocalToWorldComponent(LocalToWorldComponent const &localToWorld) const
        {
            Camera camera;
            camera.ViewMatrix = glm::inverse(localToWorld.Matrix);
            camera.Properties = Properties;

            return camera;
        }
    };
}