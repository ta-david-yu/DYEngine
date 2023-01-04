#pragma once

#include "Graphics/RenderPipelineBase.h"
#include "Graphics/MaterialPropertyBlock.h"

#include <vector>

namespace DYE
{
	class Texture2D;

	class RenderPipeline2D : public RenderPipelineBase
	{
	public:
		struct RenderSubmission2D
		{
			std::shared_ptr<VertexArray> VertexArray;
			std::shared_ptr<Material> Material;
			glm::mat4 ObjectToWorldMatrix;
			MaterialPropertyBlock MaterialPropertyBlock;
		};

		RenderPipeline2D();
		RenderPipeline2D(RenderPipeline2D const& other) = delete;
		void Submit(std::shared_ptr<VertexArray> &vertexArray, std::shared_ptr<Material> &material, glm::mat4 objectToWorldMatrix) override;
		void SubmitSprite(const std::shared_ptr<Texture2D>& texture, glm::vec4 color, glm::mat4 objectToWorldMatrix);
		void SubmitTiledSprite(const std::shared_ptr<Texture2D>& texture, glm::vec4 tilingOffset, glm::vec4 color, glm::mat4 objectToWorldMatrix);

	protected:
		void onPreRender() override;
		void renderCamera(CameraProperties const& camera) override;
		void onPostRender() override;

	private:
		std::vector<RenderSubmission2D> m_Submissions;

		std::shared_ptr<VertexArray> m_DefaultSpriteVAO;
		std::shared_ptr<Material> m_DefaultSpriteMaterial;
	};
}