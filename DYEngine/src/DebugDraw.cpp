#include "Graphics/DebugDraw.h"

#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Buffer.h"
#include "Graphics/RenderCommand.h"

namespace DYE
{
	std::vector<glm::vec3> DebugDraw::s_BatchedVertices = {};
	std::vector<std::uint32_t> DebugDraw::s_BatchedIndices = {};
	std::shared_ptr<VertexArray> DebugDraw::s_VertexArray {};
	std::shared_ptr<ShaderProgram> DebugDraw::s_ShaderProgram {};

	void DebugDraw::initialize()
	{
		s_VertexArray = VertexArray::Create();
		{
			auto vertexBufferObject = VertexBuffer::Create(0, BufferUsageHint::StreamDraw);
			VertexLayout const vertexLayout
				{
					VertexAttribute(VertexAttributeType::Float3, "position", false)
				};

			vertexBufferObject->SetLayout(vertexLayout);
			s_VertexArray->AddVertexBuffer(vertexBufferObject);

			auto indexBufferObject = IndexBuffer::Create(0, BufferUsageHint::StreamDraw);
			s_VertexArray->SetIndexBuffer(indexBufferObject);
		}

		s_ShaderProgram = ShaderProgram::CreateFromFile("Shader_DebugGizmos", "assets\\default\\DebugGizmo.shader");
	}

	void DebugDraw::pushLineData(std::vector<glm::vec3> const& vertices, std::vector<std::uint32_t> const&indices)
	{
		s_BatchedVertices.insert(s_BatchedVertices.end(), vertices.begin(), vertices.end());
		s_BatchedIndices.insert(s_BatchedIndices.end(), indices.begin(), indices.end());
	}

	void DebugDraw::renderDebugDrawOnCamera(CameraProperties const& camera)
	{
		if (!s_VertexArray || !s_ShaderProgram)
		{
			DYE_ASSERT_RELEASE(false && "Debug Gizmo's VAO or Shader have not been created, did you forget to call DebugDraw::initialize()?");
			return;
		}

		if (s_BatchedVertices.empty() || s_BatchedIndices.empty())
		{
			// Flush the vertex & index data.
			s_BatchedVertices.clear();
			s_BatchedIndices.clear();
			return;
		}

		// Pass vertices & indices to VAO.
		auto const vertexDataSize = s_BatchedVertices.size() * sizeof (glm::vec3);
		s_VertexArray->GetVertexBuffers()[0]->ResetData(s_BatchedVertices.data(), vertexDataSize, BufferUsageHint::StreamDraw);
		s_VertexArray->GetIndexBuffer()->ResetData(s_BatchedIndices.data(), s_BatchedIndices.size(), BufferUsageHint::StreamDraw);

		// Bind gizmo shader and draw it.
		s_ShaderProgram->GetDefaultRenderState().Apply();
		s_ShaderProgram->Use();
		RenderCommand::GetInstance().DrawIndexedLinesNow(*s_VertexArray);
	}

	void DebugDraw::clearDebugDraw()
	{
		// Flush the vertex & index data.
		s_BatchedVertices.clear();
		s_BatchedIndices.clear();
	}

	void DebugDraw::Line(glm::vec3 start, glm::vec3 end)
	{
		std::uint32_t const startIndex = s_BatchedVertices.size();
		pushLineData({ start, end }, { startIndex, startIndex + 1 });
	}
}