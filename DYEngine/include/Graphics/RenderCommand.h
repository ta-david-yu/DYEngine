#pragma once

#include "Graphics/VertexArray.h"

#include <glm/glm.hpp>

namespace DYE
{
    class RenderCommand
    {
    public:
        static void Init();
        static void SetViewport(std::uint32_t x, std::uint32_t, std::uint32_t width, std::uint32_t height);
        static void SetClearColor(const glm::vec4& color);
        static void Clear();

        /// Draw
        /// \param vertexArray currently bound VAO
        /// \param indexCount number of indices to be drawn
        static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, std::uint32_t indexCount);
    };
}