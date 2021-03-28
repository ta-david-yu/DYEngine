#include "Graphics/Renderer.h"

namespace DYE
{

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::Shutdown()
    {

    }

    void Renderer::OnWindowResize(std::uint32_t width, std::uint32_t height)
    {
        /// TODO: set viewport per camera (under BeginCameraView)
        RenderCommand::SetViewport(0, 0, width, height);
    }

    /// TODO: Pass in a camera, camera also has viewport information
    void Renderer::BeginCameraView()
    {
        // TODO:
    }

    void Renderer::EndCameraView()
    {
        // TODO:
    }

    void Renderer::Submit(const std::shared_ptr<ShaderProgram> &shader, const std::shared_ptr<VertexArray> &vertexArray,
                          const glm::mat4 &transform)
    {
        // TODO: draw 3d object
    }
}