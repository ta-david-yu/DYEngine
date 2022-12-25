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
		static RenderCommand& GetInstance();

		/// Initialize internal render command instance based on the platform.
        static void InitSingleton();

		RenderCommand() = default; // TODO: delete default constructor, because eventually the base class will become abstract/interface.
		RenderCommand(const RenderCommand &) = delete;

        void SetViewport(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);
        void SetClearColor(const glm::vec4& color);

        /// Clear currently bound draw buffer (color, depth, stencil)
        void Clear();

		/// Draw Index Buffer of the given VAO. The number of indices to be drawn is based on the length of the VAO's IndexBuffer.
		/// \param vertexArray
		void DrawIndexedNow(VertexArray const& vertexArray);

		void DrawIndexedNow(RenderParameters const& renderParameters, VertexArray const& vertexArray, glm::mat4 objectToWorldMatrix);

	private:
		static std::unique_ptr<RenderCommand> s_Instance;
	};
}