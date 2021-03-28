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

        static void Submit(const std::shared_ptr<ShaderProgram>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
    };
}