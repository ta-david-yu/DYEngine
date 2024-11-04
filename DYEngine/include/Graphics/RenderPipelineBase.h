#pragma once

#include "Graphics/Camera.h"
#include "MaterialPropertyBlock.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace DYE
{
    class Material;

    class VertexArray;

    class RenderPipelineManager;

    class RenderPipelineBase
    {
        friend RenderPipelineManager;
    public:
        virtual ~RenderPipelineBase() = default;

        // Submit a geometry and a material to be rendered.
        // Whether the submission is rendered immediately, depending on the implementation details of derived RenderPipeline.
        virtual void Submit(const std::shared_ptr<VertexArray> &vertexArray, const std::shared_ptr<Material> &material,
                            glm::mat4 objectToWorldMatrix, MaterialPropertyBlock materialPropertyBlock) = 0;

    protected:
        virtual void onPreRender() = 0;
        virtual void renderCamera(const Camera &camera) = 0;
        virtual void onPostRender() = 0;
    };
}