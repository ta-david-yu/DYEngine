#pragma once

#include "Graphics/RenderCommand.h"
#include "Graphics/Shader.h"

#include <memory>

namespace DYE
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(std::uint32_t width, std::uint32_t height);

        /// TODO: Pass in a camera
        static void BeginCameraView();
        static void EndCameraView();

        /// Bind the given shader program, VAO and use the transform to draw the geometry
        /// \param shader the shader program used to draw the geometry
        /// \param vertexArray the VAO with geometry/vertices data
        /// \param transform the transform of the geometry
        static void Submit(const std::shared_ptr<ShaderProgram>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
    };
}