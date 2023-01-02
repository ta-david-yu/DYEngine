#pragma once

#include "Graphics/CameraProperties.h"

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
		virtual void Submit(std::shared_ptr<VertexArray>& vertexArray, std::shared_ptr<Material>& material, glm::mat4 objectToWorldMatrix) = 0;

	protected:
		virtual void onPreRender() = 0;
		void bindCameraSettings(CameraProperties& camera);
		virtual void renderCamera(CameraProperties const& camera) = 0;
		virtual void onPostRender() = 0;
	};
}