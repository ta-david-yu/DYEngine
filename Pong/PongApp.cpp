#include "AppEntryPoint.h"
#include "Application.h"
#include "Scene/Entity.h"
#include "Scene/Transform.h"
#include "Util/TypeUtil.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderCommand.h"

#include "PongLayer.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <yaml-cpp/yaml.h>

namespace DYE
{
    class PongApp final : public Application
    {
    public:
		PongApp() = delete;
		PongApp(const PongApp &) = delete;

        explicit PongApp(const std::string &windowName, int fixedFramePerSecond = 60)
            : Application(windowName, fixedFramePerSecond)
        {
			pushLayer(std::make_shared<PongLayer>());
        }

        ~PongApp() final = default;
    };
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new PongApp { "Pong", 60 };
}
