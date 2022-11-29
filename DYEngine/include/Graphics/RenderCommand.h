#pragma once

#include <glm/glm.hpp>

namespace DYE
{
	class VertexArray;
	class ShaderProgram;

    class RenderCommand
    {
    public:
        static void Init();
        static void SetViewport(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);
        static void SetClearColor(const glm::vec4& color);

        /// Clear currently bound draw buffer (color, depth, stencil)
        static void Clear();

        /// Draw Index Buffer of the given VAO
        /// \param vertexArray
        /// \param indexCount the number of indices to be drawn, if it's 0, will use vertexArray->GetIndexBuffer()->GetCount() instead
        static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, std::uint32_t indexCount);

		/// Draw Index Buffer of the given VAO. The number of indices to be drawn is based on the length of the VAO's IndexBuffer.
		/// \param vertexArray
		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
	};
}