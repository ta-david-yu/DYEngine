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
        RenderPipeline2D(RenderPipeline2D const &other) = delete;
        void Submit(const std::shared_ptr<VertexArray> &vertexArray, const std::shared_ptr<Material> &material,
                    glm::mat4 objectToWorldMatrix, MaterialPropertyBlock materialPropertyBlock) override;

        std::shared_ptr<VertexArray> GetDefaultQuadSpriteVAO() const { return m_DefaultSpriteVAO; }
        std::shared_ptr<Material> GetDefaultSpriteMaterial() const { return m_DefaultSpriteMaterial; }

        [[deprecated("Use Submit & GetDefaultQuadSpriteVAO to submit a quad sprite instead.")]]
        void SubmitSprite(const std::shared_ptr<Texture2D> &texture, glm::vec4 color, glm::mat4 objectToWorldMatrix);
        [[deprecated("Use Submit & GetDefaultQuadSpriteVAO to submit a quad sprite instead.")]]
        void SubmitTiledSprite(const std::shared_ptr<Texture2D> &texture, glm::vec4 tilingOffset, glm::vec4 color, glm::mat4 objectToWorldMatrix);

    protected:
        void onPreRender() override;
        void renderCamera(const Camera &camera) override;
        void onPostRender() override;

    private:
        std::vector<RenderSubmission2D> m_Submissions;

        std::shared_ptr<VertexArray> m_DefaultSpriteVAO;
        std::shared_ptr<Material> m_DefaultSpriteMaterial;
    };
}