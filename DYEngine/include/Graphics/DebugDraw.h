#pragma once

#include "Graphics/CameraProperties.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		struct GeometryInstancedArrays
		{
			int NumberOfInstances = 0;
			std::vector<float> Colors;
			std::vector<float> ModelMatrices;

			/// The VBO which stores per-instance color is located at index 2 in Geometry VAO.
			constexpr const static std::uint32_t ColorVBOIndex = 1;

			/// The VBO which stores per-instance model matrix is located at index 1 in Geometry VAO.
			constexpr const static std::uint32_t ModelMatrixVBOIndex = 2;

			void AddInstance(glm::vec4 color, glm::mat4 modelMatrix)
			{
				Colors.insert(Colors.end(), glm::value_ptr(color), glm::value_ptr(color) + 4);
				ModelMatrices.insert(ModelMatrices.end(), glm::value_ptr(modelMatrix), glm::value_ptr(modelMatrix) + 16);
				NumberOfInstances++;
			}

			void Clear()
			{
				ModelMatrices.clear();
				Colors.clear();
				NumberOfInstances = 0;
			}
		};

		static std::shared_ptr<ShaderProgram> s_LineGizmoShaderProgram;
		static std::shared_ptr<ShaderProgram> s_GeometryGizmoShaderProgram;

		static std::shared_ptr<VertexArray> s_BatchedLineVAO;
		static std::vector<float> s_BatchedLineVertices;
		static std::vector<std::uint32_t> s_BatchedLineIndices;

		static std::shared_ptr<VertexArray> s_CubeVAO;
		static GeometryInstancedArrays s_CubeInstancedArrays;

		static std::shared_ptr<VertexArray> s_WireCubeVAO;
		static GeometryInstancedArrays s_WireCubeInstancedArrays;

		static std::shared_ptr<VertexArray> s_CircleVAO;
		static GeometryInstancedArrays s_CircleInstancedArrays;

		static void initialize();
		static std::shared_ptr<VertexArray> createBatchedLineVAO();
		static std::shared_ptr<VertexArray> createCubeVAO();
		static std::shared_ptr<VertexArray> createWireCubeVAO();
		static std::shared_ptr<VertexArray> createWireCircleVAO();

		static void renderDebugDrawOnCamera(CameraProperties const& camera);
		static void renderBatchedLineVAO(CameraProperties const& camera);
		static void renderLineGeometryVAO(VertexArray const& vao, GeometryInstancedArrays & instancedArrays);
		static void renderTriangleGeometryVAO(const VertexArray &vao, DebugDraw::GeometryInstancedArrays &instancedArrays);

		static void clearDebugDraw();

	public:
		[[deprecated("Line function is still buggy, avoid calling it more than 100 times a frame.")]]
		static void Line(glm::vec3 start, glm::vec3 end, glm::vec4 color);
		static void Cube(glm::vec3 center, glm::vec3 size, glm::vec4 color);
		static void AABB(glm::vec3 min, glm::vec3 max, glm::vec4 color);
		static void Circle(glm::vec3 center, float radius, glm::vec3 up, glm::vec4 color);
		static void Sphere(glm::vec3 center, float radius, glm::vec4 color);
	};
}

