#include "AppEntryPoint.h"
#include "Core/Application.h"
#include "Scene/Entity.h"
#include "Scene/SceneLayer.h"
#include "Scene/Transform.h"
#include "Util/TypeUtil.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderCommand.h"

#include "Scene/ImageRenderer.h"

#include "ImagePointerEventHandler.h"
#include "SandboxLayer.h"
#include "FrameCounterComponent.h"
#include "SubtitleUpdater.h"
#include "SandboxMessage.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <yaml-cpp/yaml.h>

namespace DYE
{
    class SandboxApp final : public Application
    {
    public:
        SandboxApp() = delete;
        SandboxApp(const SandboxApp &) = delete;

        explicit SandboxApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
			pushLayerImmediate(std::make_shared<SandboxLayer>());
        }

        ~SandboxApp() final = default;
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new SandboxApp { "Sandbox", 60 };
}
