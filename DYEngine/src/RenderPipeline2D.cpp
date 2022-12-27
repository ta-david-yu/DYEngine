#include "Graphics/RenderPipeline2D.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/Material.h"
#include "Util/Algorithm.h"

namespace DYE
{
	void RenderPipeline2D::render(const std::vector<CameraProperties> &cameras)
	{
		// Render with each camera.
		for (auto const& camera : cameras)
		{
			// Sort the submission (render queue, sorting layer, camera distance etc).
			// TODO: insertion sort cuz the vector will already be kinda sorted in every Submit call
			glm::mat4 const viewMatrix = camera.GetViewMatrix();
			std::stable_sort
			(
				m_Submissions.begin(),
				m_Submissions.end(),
				[viewMatrix](RenderSubmission2D const& submissionA, RenderSubmission2D const& submissionB)
				{
					auto modelViewMatrixA = viewMatrix * submissionA.ObjectToWorldMatrix;
					auto modelViewMatrixB = viewMatrix * submissionB.ObjectToWorldMatrix;
					glm::vec4 const viewPositionA = modelViewMatrixA * glm::vec4 {0, 0, 0, 1};
					glm::vec4 const viewPositionB = modelViewMatrixB * glm::vec4 {0, 0, 0, 1};
					return viewPositionA.z < viewPositionB.z;
				}
			);

			// Execute draw-calls.
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

		/*
		Algorithm::InsertionSort
		(
			m_Submissions.begin(),
			m_Submissions.end(),
			[](auto const& submissionA, auto const& submissionB)
			{
				return true;
			}
		);*/
	}
}