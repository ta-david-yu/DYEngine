#pragma once

#include "Graphics/CameraProperties.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace DYE
{
	class RenderPipelineManager;
	class VertexArray;
	class ShaderProgram;

	class DebugDraw
	{
		friend RenderPipelineManager;

	private:
		static std::vector<glm::vec3> s_BatchedVertices;
		static std::vector<std::uint32_t> s_BatchedIndices;

		static std::shared_ptr<VertexArray> s_VertexArray;
		static std::shared_ptr<ShaderProgram> s_ShaderProgram;

		static void initialize();
		static void pushLineData(std::vector<glm::vec3> const& vertices, std::vector<std::uint32_t> const& indices);
		static void renderDebugDrawOnCamera(CameraProperties const& camera);
		static void clearDebugDraw();

	public:
		static void Line(glm::vec3 start, glm::vec3 end);
	};
}

