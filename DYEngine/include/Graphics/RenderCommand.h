#pragma once

#include "Graphics/Camera.h"
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
		Camera Camera;
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

		glm::vec<2, std::uint32_t> GetMaxFramebufferSize() const;

		/// Before calling this function, you have to call window.MakeCurrent() first.
		void SwapWindowBuffer(WindowBase const& windowBase);

        void SetViewport(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);
		void SetViewport(Math::Rect viewportRect);
        void SetClearColor(const glm::vec4& color);
		void SetLinePrimitiveWidth(float width);

        /// Clear currently bound draw buffer (color, depth, stencil)
        void Clear();

		/// Draw VAO as line primitives.
		/// \param vertexArray
		void DrawIndexedLinesNow(const VertexArray &vertexArray);

		/// Draw multiple VAO instances as line primitives.
		/// \param vertexArray
		/// \param numberOfInstances the number of instances to be drawn
		void DrawIndexedLinesInstancedNow(const VertexArray &vertexArray, int numberOfInstances);

		/// Draw VAO as triangle primitives.
		/// \param vertexArray
		void DrawIndexedNow(VertexArray const& vertexArray);

		/// Draw multiple VAO instances as triangle primitives.
		/// \param vertexArray
		/// \param numberOfInstances the number of instances to be drawn
		void DrawIndexedInstancedNow(const VertexArray &vertexArray, int numberOfInstances);

		/// Draw VAO as triangle primitives with the given parameters.
		/// \param renderParameters
		/// \param vertexArray
		/// \param objectToWorldMatrix
		void DrawIndexedNow(RenderParameters const& renderParameters, VertexArray const& vertexArray, glm::mat4 objectToWorldMatrix);
	};
}