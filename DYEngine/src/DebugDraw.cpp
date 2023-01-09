#include "Graphics/DebugDraw.h"

#include "Math/Math.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Buffer.h"
#include "Graphics/RenderCommand.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DYE
{
	std::shared_ptr<ShaderProgram> DebugDraw::s_LineGizmoShaderProgram {};
	std::shared_ptr<ShaderProgram> DebugDraw::s_GeometryGizmoShaderProgram {};

	std::shared_ptr<VertexArray> DebugDraw::s_BatchedLineVAO {};
	std::vector<float> DebugDraw::s_BatchedLineVertices = {};
	std::vector<std::uint32_t> DebugDraw::s_BatchedLineIndices = {};

	std::shared_ptr<VertexArray> DebugDraw::s_CubeVAO {};
	DebugDraw::GeometryInstancedArrays DebugDraw::s_CubeInstancedArrays {};

	std::shared_ptr<VertexArray> DebugDraw::s_WireCubeVAO {};
	DebugDraw::GeometryInstancedArrays DebugDraw::s_WireCubeInstancedArrays {};

	std::shared_ptr<VertexArray> DebugDraw::s_CircleVAO {};
	DebugDraw::GeometryInstancedArrays DebugDraw::s_CircleInstancedArrays {};

	void DebugDraw::initialize()
	{
		s_LineGizmoShaderProgram = ShaderProgram::CreateFromFile("Shader_DebugLineGizmo", "assets\\default\\DebugLineGizmo.shader");
		s_GeometryGizmoShaderProgram = ShaderProgram::CreateFromFile("Shader_DebugGeometryGizmo", "assets\\default\\DebugGeometryGizmo.shader");

		s_BatchedLineVAO = createBatchedLineVAO();
		s_CubeVAO = createCubeVAO();
		s_WireCubeVAO = createWireCubeVAO();
		s_CircleVAO = createWireCircleVAO();
	}

	std::shared_ptr<VertexArray> DebugDraw::createBatchedLineVAO()
	{
		auto vao = VertexArray::Create();
		{
			auto vertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float3, "position", false),
					VertexAttribute(VertexAttributeType::Float4, "color", false)
				};

			vertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(vertexBufferObject);

			auto indexBufferObject = IndexBuffer::Create(0, BufferUsageHint::StreamDraw);
			vao->SetIndexBuffer(indexBufferObject);
		}

		return std::move(vao);
	}

	std::shared_ptr<VertexArray> DebugDraw::createCubeVAO()
	{
		auto vao = VertexArray::Create();

		// Create the mesh vertex data and indices
		{
			// Create vertices [position]
			//				   [       3] = 3 elements per vertex
			float vertices[3 * 8] =
				{
					// Upper rect
					0.5f, 0.5f, 0.5f,
					-0.5f, 0.5f, 0.5f,
					-0.5f, 0.5f, -0.5f,
					0.5f, 0.5f, -0.5f,

					// Lower rect
					0.5f, -0.5f, 0.5f,
					-0.5f, -0.5f, 0.5f,
					-0.5f, -0.5f, -0.5f,
					0.5f, -0.5f, -0.5f,
				};


			std::array<std::uint32_t, 36> indices =
				{
					0, 3, 1,
					3, 2, 1,

					2, 3, 7,
					7, 6, 2,

					3, 0, 7,
					0, 4, 7,

					5, 0, 1,
					4, 0, 5,

					5, 1, 6,
					6, 1, 2,

					7, 4, 5,
					6, 7, 5
				};

			auto vertexPositionBufferObject = VertexBuffer::Create(vertices, sizeof(vertices));
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float3, "position", false)
				};

			vertexPositionBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(vertexPositionBufferObject);

			auto indexBufferObject = IndexBuffer::Create(indices.data(), indices.size());
			vao->SetIndexBuffer(indexBufferObject);
		}

		// Create vertex buffer object for per instance color
		{
			auto instancedVertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);

			// We set the divisor to 1, so the vertex attribute only strides when a new instance is being rendered.
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float4, "perInstanceColor", false, 1)
				};

			instancedVertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(instancedVertexBufferObject);
		}

		// Create vertex buffer object for per instance model matrix
		{
			auto instancedVertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);

			// We set the divisor to 1, so the vertex attribute only strides when a new instance is being rendered.
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Mat4, "perInstanceModelMatrix", false, 1)
				};

			instancedVertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(instancedVertexBufferObject);
		}

		return std::move(vao);
	}

	std::shared_ptr<VertexArray> DebugDraw::createWireCubeVAO()
	{
		auto vao = VertexArray::Create();

		// Create the mesh vertex data and indices
		{
			// Create vertices [position]
			//				   [       3] = 3 elements per vertex
			float vertices[3 * 8] =
				{
					// Upper rect
					0.5f, 0.5f, 0.5f,
					-0.5f, 0.5f, 0.5f,
					-0.5f, 0.5f, -0.5f,
					0.5f, 0.5f, -0.5f,

					// Lower rect
					0.5f, -0.5f, 0.5f,
					-0.5f, -0.5f, 0.5f,
					-0.5f, -0.5f, -0.5f,
					0.5f, -0.5f, -0.5f,
				};


			std::array<std::uint32_t, 24> indices =
				{
					0, 1,
					1, 2,
					2, 3,
					3, 0,

					0, 4,
					1, 5,
					2, 6,
					3, 7,

					4, 5,
					5, 6,
					6, 7,
					7, 4
				};

			auto vertexPositionBufferObject = VertexBuffer::Create(vertices, sizeof(vertices));
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float3, "position", false)
				};

			vertexPositionBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(vertexPositionBufferObject);

			auto indexBufferObject = IndexBuffer::Create(indices.data(), indices.size());
			vao->SetIndexBuffer(indexBufferObject);
		}

		// Create vertex buffer object for per instance color
		{
			auto instancedVertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);

			// We set the divisor to 1, so the vertex attribute only strides when a new instance is being rendered.
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float4, "perInstanceColor", false, 1)
				};

			instancedVertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(instancedVertexBufferObject);
		}

		// Create vertex buffer object for per instance model matrix
		{
			auto instancedVertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);

			// We set the divisor to 1, so the vertex attribute only strides when a new instance is being rendered.
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Mat4, "perInstanceModelMatrix", false, 1)
				};

			instancedVertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(instancedVertexBufferObject);
		}

		return std::move(vao);
	}

	std::shared_ptr<VertexArray> DebugDraw::createWireCircleVAO()
	{
		auto vao = VertexArray::Create();

		// Create the mesh vertex data and indices
		{
			std::vector<float> vertices;
			std::vector<std::uint32_t> indices;

			constexpr int const numberOfSides = 36;
			constexpr float const angleDegreeOffset = 360.0f / numberOfSides;

			// We might want to add stacks of circles here, therefore we keep the startIndex here.
			std::uint32_t const startIndex = vertices.size();
			for (int i = 0; i < numberOfSides; i++)
			{
				float const startAngleRadian = glm::radians(angleDegreeOffset * (float) i);
				glm::vec3 start = {glm::cos(startAngleRadian), glm::sin(startAngleRadian), 0};
				vertices.insert(vertices.end(), glm::value_ptr(start), glm::value_ptr(start) + 3);

				std::uint32_t const firstIndex = startIndex + i;
				std::uint32_t const secondIndex = startIndex + (i + 1) % numberOfSides;
				indices.insert(indices.end(), {firstIndex, secondIndex});
			}

			auto vertexPositionBufferObject = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float));
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float3, "position", false)
				};

			vertexPositionBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(vertexPositionBufferObject);

			auto indexBufferObject = IndexBuffer::Create(indices.data(), indices.size());
			vao->SetIndexBuffer(indexBufferObject);
		}

		// Create vertex buffer object for per instance color
		{
			auto instancedVertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);

			// We set the divisor to 1, so the vertex attribute only strides when a new instance is being rendered.
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float4, "perInstanceColor", false, 1)
				};

			instancedVertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(instancedVertexBufferObject);
		}

		// Create vertex buffer object for per instance model matrix
		{
			auto instancedVertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);

			// We set the divisor to 1, so the vertex attribute only strides when a new instance is being rendered.
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Mat4, "perInstanceModelMatrix", false, 1)
				};

			instancedVertexBufferObject->SetLayout(vertexLayout);
			vao->AddVertexBuffer(instancedVertexBufferObject);
		}

		return std::move(vao);
	}

	void DebugDraw::renderDebugDrawOnCamera(CameraProperties const& camera)
	{
		if (!s_LineGizmoShaderProgram || !s_GeometryGizmoShaderProgram)
		{
			DYE_ASSERT_RELEASE(false && "Debug Gizmo's VAO or Shader have not been created, did you forget to call DebugDraw::initialize()?");
			return;
		}

		renderBatchedLineVAO(camera);

		s_GeometryGizmoShaderProgram->GetDefaultRenderState().Apply();
		s_GeometryGizmoShaderProgram->Use();

		// View matrix
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		auto viewMatrixLoc = glGetUniformLocation(s_GeometryGizmoShaderProgram->GetID(), DefaultUniformNames::ViewMatrix);
		glCall(glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix)));

		// Projection matrix
		float const aspectRatio = camera.GetAspectRatio();
		glm::mat4 projectionMatrix = camera.GetProjectionMatrix(aspectRatio);
		auto projectionMatrixLoc = glGetUniformLocation(s_GeometryGizmoShaderProgram->GetID(), DefaultUniformNames::ProjectionMatrix);
		glCall(glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));

		renderTriangleGeometryVAO(*s_CubeVAO, s_CubeInstancedArrays);
		renderLineGeometryVAO(*s_WireCubeVAO, s_WireCubeInstancedArrays);
		renderLineGeometryVAO(*s_CircleVAO, s_CircleInstancedArrays);
	}

	void DebugDraw::renderBatchedLineVAO(CameraProperties const& camera)
	{
		if (s_BatchedLineVertices.empty() || s_BatchedLineIndices.empty())
		{
			return;
		}

		s_LineGizmoShaderProgram->GetDefaultRenderState().Apply();
		s_LineGizmoShaderProgram->Use();

		// View matrix
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		auto viewMatrixLoc = glGetUniformLocation(s_LineGizmoShaderProgram->GetID(), DefaultUniformNames::ViewMatrix);
		glCall(glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix)));

		// Projection matrix
		float const aspectRatio = camera.GetAspectRatio();
		glm::mat4 projectionMatrix = camera.GetProjectionMatrix(aspectRatio);
		auto projectionMatrixLoc = glGetUniformLocation(s_LineGizmoShaderProgram->GetID(), DefaultUniformNames::ProjectionMatrix);
		glCall(glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));

		// Pass vertices & indices to VBO/EBO.
		auto const vertexDataSize = s_BatchedLineVertices.size() * sizeof (float) * 7;  // pos + color = 3 floats + 4 floats = 7 floats
		s_BatchedLineVAO->GetVertexBuffers()[0]->ResetData(s_BatchedLineVertices.data(), vertexDataSize, BufferUsageHint::StreamDraw);
		s_BatchedLineVAO->GetIndexBuffer()->ResetData(s_BatchedLineIndices.data(), s_BatchedLineIndices.size(), BufferUsageHint::StreamDraw);

		RenderCommand::GetInstance().DrawIndexedLinesNow(*s_BatchedLineVAO);
	}

	void DebugDraw::renderLineGeometryVAO(const VertexArray &vao, DebugDraw::GeometryInstancedArrays &instancedArrays)
	{
		if (instancedArrays.NumberOfInstances == 0)
		{
			return;
		}

		// Set per instance data: Color & Model Matrix
		auto const& perInstanceColorVBO = vao.GetVertexBuffers()[GeometryInstancedArrays::ColorVBOIndex];
		perInstanceColorVBO->ResetData
			(
				instancedArrays.Colors.data(),
				instancedArrays.Colors.size() * sizeof (float),
				BufferUsageHint::StreamDraw
			);

		auto const& perInstanceModelMatrixVBO = vao.GetVertexBuffers()[GeometryInstancedArrays::ModelMatrixVBOIndex];
		perInstanceModelMatrixVBO->ResetData
			(
				instancedArrays.ModelMatrices.data(),
				instancedArrays.ModelMatrices.size() * sizeof (float),
				BufferUsageHint::StreamDraw
			);

		RenderCommand::GetInstance().DrawIndexedLinesInstancedNow(vao, instancedArrays.NumberOfInstances);
	}

	void DebugDraw::renderTriangleGeometryVAO(const VertexArray &vao, DebugDraw::GeometryInstancedArrays &instancedArrays)
	{
		if (instancedArrays.NumberOfInstances == 0)
		{
			return;
		}

		// Set per instance data: Color & Model Matrix
		auto const& perInstanceColorVBO = vao.GetVertexBuffers()[GeometryInstancedArrays::ColorVBOIndex];
		perInstanceColorVBO->ResetData
			(
				instancedArrays.Colors.data(),
				instancedArrays.Colors.size() * sizeof (float),
				BufferUsageHint::StreamDraw
			);

		auto const& perInstanceModelMatrixVBO = vao.GetVertexBuffers()[GeometryInstancedArrays::ModelMatrixVBOIndex];
		perInstanceModelMatrixVBO->ResetData
			(
				instancedArrays.ModelMatrices.data(),
				instancedArrays.ModelMatrices.size() * sizeof (float),
				BufferUsageHint::StreamDraw
			);

		RenderCommand::GetInstance().DrawIndexedInstancedNow(vao, instancedArrays.NumberOfInstances);
	}

	void DebugDraw::clearDebugDraw()
	{
		s_BatchedLineVertices.clear();
		s_BatchedLineIndices.clear();

		s_CubeInstancedArrays.Clear();
		s_WireCubeInstancedArrays.Clear();
		s_CircleInstancedArrays.Clear();
	}

	void DebugDraw::Line(glm::vec3 start, glm::vec3 end, glm::vec4 color)
	{
		std::uint32_t const startIndex = s_BatchedLineVertices.size() / 7;

		s_BatchedLineVertices.insert(s_BatchedLineVertices.end(), glm::value_ptr(start), glm::value_ptr(start) + 3);
		s_BatchedLineVertices.insert(s_BatchedLineVertices.end(), glm::value_ptr(color), glm::value_ptr(color) + 4);

		s_BatchedLineVertices.insert(s_BatchedLineVertices.end(), glm::value_ptr(end), glm::value_ptr(end) + 3);
		s_BatchedLineVertices.insert(s_BatchedLineVertices.end(), glm::value_ptr(color), glm::value_ptr(color) + 4);

		s_BatchedLineIndices.insert(s_BatchedLineIndices.end(), {startIndex, startIndex + 1});
	}

	void DebugDraw::Cube(glm::vec3 center, glm::vec3 size, glm::vec4 color)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, center);
		modelMatrix = glm::scale(modelMatrix, size);

		s_CubeInstancedArrays.AddInstance(color * 0.4f, modelMatrix);
		s_WireCubeInstancedArrays.AddInstance(color, modelMatrix);
	}

	void DebugDraw::AABB(glm::vec3 min, glm::vec3 max, glm::vec4 color)
	{
		glm::vec3 const center = (min + max) * 0.5f;
		glm::vec3 const size = max - min;

		Cube(center, size, color);
	}

	void DebugDraw::Circle(glm::vec3 center, float radius, glm::vec3 up, glm::vec4 color)
	{
		float const scale = radius;

		glm::vec3 const defaultUp = glm::vec3 {0, 0, 1};
		glm::quat const rotation = Math::FromToRotation(defaultUp, up);

		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, center);
		modelMatrix = modelMatrix * glm::toMat4(rotation);
		modelMatrix = glm::scale(modelMatrix, {scale, scale ,scale});

		s_CircleInstancedArrays.AddInstance(color, modelMatrix);
	}

	void DebugDraw::Sphere(glm::vec3 center, float radius, glm::vec4 color)
	{
		DebugDraw::Circle(center, radius, {1, 0, 0}, color);
		DebugDraw::Circle(center, radius, {0, 1, 0}, color);
		DebugDraw::Circle(center, radius, {0, 0, 1}, color);
	}
}