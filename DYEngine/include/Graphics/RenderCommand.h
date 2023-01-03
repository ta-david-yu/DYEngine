#pragma once

#include "Graphics/CameraProperties.h"
#include "Graphics/MaterialPropertyBlock.h"
#include "Math/Rect.h"

#include <glm/glm.hpp>

#include <memory>

namespace DYE
{
	class Material;
	class VertexArray;
	class ShaderProgram;
	class WindowBase;

	struct RenderParameters
	{
		CameraProperties Camera;
		std::shared_ptr<Material> Material;
		MaterialPropertyBlock PropertyBlock;
	};

    class RenderCommand
    {
	private:
		static std::unique_ptr<RenderCommand> s_Instance;

    public:
		static RenderCommand& GetInstance();

		/// Initialize internal render command instance based on the platform.
        static void InitSingleton();

		RenderCommand() = default; // TODO: delete default constructor, because eventually the base class will become abstract/interface.
		RenderCommand(const RenderCommand &) = delete;

		/// Swap the front and back buffers of the given window if it's double-buffered.
		/// You would have to make the context of the given window's current first.
		void SwapWindowBuffer(WindowBase const& windowBase);

        void SetViewport(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);
		void SetViewport(Math::Rect viewportRect);
        void SetClearColor(const glm::vec4& color);

        /// Clear currently bound draw buffer (color, depth, stencil)
        void Clear();

		/// Draw Index Buffer of the given VAO. The number of indices to be drawn is based on the length of the VAO's IndexBuffer.
		/// \param vertexArray
		void DrawIndexedNow(VertexArray const& vertexArray);

		/// Execute draw call.
		/// \param renderParameters
		/// \param vertexArray
		/// \param objectToWorldMatrix
		void DrawIndexedNow(RenderParameters const& renderParameters, VertexArray const& vertexArray, glm::mat4 objectToWorldMatrix);
	};
}