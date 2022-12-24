#pragma once

#include "Graphics/CameraProperties.h"

#include <glm/glm.hpp>

#include <memory>


namespace DYE
{
	class Material;
	class VertexArray;
	class ShaderProgram;

	struct RenderParameters
	{
		CameraProperties Camera;
		std::shared_ptr<Material> Material;

		// TODO: removed these! They should be stored in Camera.
		float AspectRatio;
	};

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
        static void DrawIndexedNow(VertexArray const& vertexArray, std::uint32_t indexCount);

		/// Draw Index Buffer of the given VAO. The number of indices to be drawn is based on the length of the VAO's IndexBuffer.
		/// \param vertexArray
		static void DrawIndexedNow(VertexArray const& vertexArray);

		static void DrawIndexedNow(RenderParameters const& renderParameters, VertexArray const& vertexArray, glm::mat4 objectToWorldMatrix);
	};
}