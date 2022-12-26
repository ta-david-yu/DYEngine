#include "Graphics/RenderPipeline2D.h"

#include "Graphics/RenderCommand.h"

namespace DYE
{
	void RenderPipeline2D::render(const std::vector<CameraProperties> &cameras)
	{
		// Render with each camera.
		for (auto const& camera : cameras)
		{
			// Sort the submission (render queue, sorting layer, camera distance etc).
			// TODO:

			// Draw calls.
			for (auto const &submission: m_Submissions)
			{
				RenderCommand::GetInstance().DrawIndexedNow
					(
						RenderParameters { .Camera = camera, .Material = submission.Material },
						*submission.VertexArray,
						submission.ObjectToWorldMatrix
					);
			}
		}

		// Clean up the submissions.
		m_Submissions.clear();
	}

	void RenderPipeline2D::Submit(
		std::shared_ptr<VertexArray> &vertexArray,
		std::shared_ptr<Material> &material,
		glm::mat4 objectToWorldMatrix)
	{
		m_Submissions.push_back
			(
				RenderSubmission2D { .VertexArray = vertexArray, .Material = material, .ObjectToWorldMatrix = objectToWorldMatrix }
			);
	}
}