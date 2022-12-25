#include "Graphics/Renderer.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"

namespace DYE
{
    void Renderer::Init()
    {
    }

    void Renderer::Shutdown()
    {

    }

    void Renderer::OnWindowResize(std::uint32_t width, std::uint32_t height)
    {
        /// TODO: set viewport per camera (under BeginCameraView)
		RenderCommand::GetInstance().SetViewport(0, 0, width, height);
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
        // TODO: draw 3d object, set VP matrix uniform
        shader->Use();
        //shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        //shader->SetMat4("u_Transform", transform);
        vertexArray->Bind();
		RenderCommand::GetInstance().DrawIndexedNow(*vertexArray.get());
    }

//    void Renderer::Submit(const std::shared_ptr<Material>& material, const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f))
//    {
//        const auto& shader = material->GetShader();
//        shader->Use();
//        shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
//        shader->SetMat4("u_Transform", transform);
//        material->SetUniforms();
//
//        const auto& vertexArray = mesh->GetVertexArray();
//        RenderCommand::DrawIndexedNow(vertexArray);
//    }
}