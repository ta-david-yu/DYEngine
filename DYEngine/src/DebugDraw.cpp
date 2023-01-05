#include "Graphics/DebugDraw.h"

#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Buffer.h"
#include "Graphics/RenderCommand.h"

#include <glm/gtc/type_ptr.hpp>

namespace DYE
{
	std::shared_ptr<ShaderProgram> DebugDraw::s_ShaderProgram {};

	std::shared_ptr<VertexArray> DebugDraw::s_CubeVAO {};
	DebugDraw::GeometryInstancedArrays DebugDraw::s_CubeInstancedArrays;

	void DebugDraw::initialize()
	{
		s_ShaderProgram = ShaderProgram::CreateFromFile("Shader_DebugGizmos", "assets\\default\\DebugGizmo.shader");

		s_CubeVAO = createWireCubeVAO();
	}

	void DebugDraw::renderDebugDrawOnCamera(CameraProperties const& camera)
	{
		if (!s_ShaderProgram)
		{
			DYE_ASSERT_RELEASE(false && "Debug Gizmo's VAO or Shader have not been created, did you forget to call DebugDraw::initialize()?");
			return;
		}

		// Bind gizmo shader and draw it.
		s_ShaderProgram->GetDefaultRenderState().Apply();
		s_ShaderProgram->Use();

		// View Matrix: World space to camera space
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		auto viewMatrixLoc = glGetUniformLocation(s_ShaderProgram->GetID(), DefaultUniformNames::ViewMatrix);
		glCall(glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix)));

		// Projection Matrix: Camera space to clip space
		float const aspectRatio = camera.GetAspectRatio();
		glm::mat4 projectionMatrix = camera.GetProjectionMatrix(aspectRatio);
		auto projectionMatrixLoc = glGetUniformLocation(s_ShaderProgram->GetID(), DefaultUniformNames::ProjectionMatrix);
		glCall(glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));

		// Set per instance data: Color & Model Matrix
		auto const& perInstanceColorVBO = s_CubeVAO->GetVertexBuffers()[GeometryInstancedArrays::ColorVBOIndex];
		perInstanceColorVBO->ResetData
			(
				s_CubeInstancedArrays.Colors.data(),
				s_CubeInstancedArrays.Colors.size() * sizeof (float),
				BufferUsageHint::StreamDraw
			);

		auto const& perInstanceModelMatrixVBO = s_CubeVAO->GetVertexBuffers()[GeometryInstancedArrays::ModelMatrixVBOIndex];
		perInstanceModelMatrixVBO->ResetData
			(
				s_CubeInstancedArrays.ModelMatrices.data(),
				s_CubeInstancedArrays.ModelMatrices.size() * sizeof (float),
				BufferUsageHint::StreamDraw
			);

		RenderCommand::GetInstance().DrawIndexedLinesInstancedNow(*s_CubeVAO, s_CubeInstancedArrays.NumberOfInstances);
	}

	void DebugDraw::clearDebugDraw()
	{
		s_CubeInstancedArrays.Clear();
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

	void DebugDraw::Cube(glm::vec3 center, glm::vec4 color)
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, center);

		s_CubeInstancedArrays.AddInstance(color, modelMatrix);
	}
}