#pragma once

#include "Graphics/RenderPipelineBase.h"

#include <vector>

namespace DYE
{
	class RenderPipeline2D : public RenderPipelineBase
	{
	public:
		struct RenderSubmission2D
		{
			std::shared_ptr<VertexArray> VertexArray;
			std::shared_ptr<Material> Material;
			glm::mat4 ObjectToWorldMatrix;
		};

		void Submit(std::shared_ptr<VertexArray> &vertexArray, std::shared_ptr<Material> &material, glm::mat4 objectToWorldMatrix) override;

	protected:
		void render(const std::vector<CameraProperties> &cameras) override;

	private:
		std::vector<RenderSubmission2D> m_Submissions;
	};
}